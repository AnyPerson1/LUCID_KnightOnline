using System;
using System.Collections.ObjectModel;
using LUCID.Controls;
using LUCID.Models;
using System.IO;
using System.IO.Pipes;
using System.Text;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;
using System.Threading.Tasks;
using System.Threading;
using Avalonia.Threading;

namespace LUCID.ViewModels
{
    public partial class MainWindowViewModel : ReactiveObject
    {
        private const string PipeName = "LUCID_MacroPipe";
        private NamedPipeServerStream _pipeServer;
        private readonly SemaphoreSlim _serverSemaphore = new SemaphoreSlim(1, 1);
        private volatile bool _isServerRunning = false;
        private volatile bool _isClientConnected = false;
        private CancellationTokenSource _serverCancellationToken;
        
        public event Action<string> ConnectionStatusChanged;

        private void InvokeConnectionStatusChanged(string message)
        {
            Console.WriteLine($"[PIPE SERVER] {DateTime.Now:HH:mm:ss.fff} - {message}");
            
            try
            {
                Dispatcher.UIThread.InvokeAsync(() => ConnectionStatusChanged?.Invoke(message));
            }
            catch (Exception ex)
            {
                Console.WriteLine($"[UI ERROR] {DateTime.Now:HH:mm:ss.fff} - UI Event hatası: {ex.Message}");
            }
        }

        public async Task CreatePipeServer()
        {
            await _serverSemaphore.WaitAsync();
            try
            {
                if (_isServerRunning)
                {
                    InvokeConnectionStatusChanged("Server zaten çalışıyor.");
                    return;
                }

                _serverCancellationToken = new CancellationTokenSource();
                await StartServerAsync();
            }
            finally
            {
                _serverSemaphore.Release();
            }
        }
        
        private async Task StartServerAsync()
        {
            try
            {
                // Önceki pipe instance'ını temizle
                if (_pipeServer != null)
                {
                    try
                    {
                        if (_pipeServer.IsConnected)
                            _pipeServer.Disconnect();
                        _pipeServer.Dispose();
                    }
                    catch { }
                    _pipeServer = null;
                }

                InvokeConnectionStatusChanged("Named Pipe Server başlatılıyor...");

                for (int attempt = 1; attempt <= 5; attempt++)
                {
                    try
                    {
                        InvokeConnectionStatusChanged($"Server oluşturma denemesi {attempt}/5");
                        
                        _pipeServer = new NamedPipeServerStream(
                            PipeName,
                            PipeDirection.Out,  // Sadece gönderme için
                            1,
                            PipeTransmissionMode.Byte,
                            PipeOptions.Asynchronous,
                            512, 
                            0     
                        );

                        _isServerRunning = true;
                        InvokeConnectionStatusChanged($"Server başarıyla oluşturuldu: {PipeName}");
                        break;
                    }
                    catch (IOException ex) when (ex.Message.Contains("busy") || ex.Message.Contains("meşgul"))
                    {
                        InvokeConnectionStatusChanged($"Pipe meşgul (deneme {attempt}/5), 1 saniye bekleniyor...");
                        await Task.Delay(1000);
                        
                        if (attempt == 5)
                        {
                            InvokeConnectionStatusChanged("Maksimum deneme sayısına ulaşıldı. Pipe temizleniyor...");
                            
                            await CleanupSystemPipes();
                            throw;
                        }
                    }
                    catch (Exception ex)
                    {
                        InvokeConnectionStatusChanged($"Server oluşturma hatası (deneme {attempt}): {ex.Message}");
                        if (attempt == 5)
                            throw;
                        await Task.Delay(1000);
                    }
                }
                
                if (_isServerRunning && _pipeServer != null)
                {
                    _ = Task.Run(() => ListenForClientsAsync(_serverCancellationToken.Token));
                }
            }
            catch (Exception ex)
            {
                InvokeConnectionStatusChanged($"Server başlatma kritik hatası: {ex.Message}");
                _isServerRunning = false;
                try
                {
                    _pipeServer?.Dispose();
                    _pipeServer = null;
                }
                catch { }
            }
        }

        private async Task CleanupSystemPipes()
        {
            try
            {
                InvokeConnectionStatusChanged("Sistem pipe'larını temizlemeye çalışılıyor...");
                await Task.Delay(2000);
                GC.Collect();
                GC.WaitForPendingFinalizers();
                InvokeConnectionStatusChanged("Pipe cleanup tamamlandı.");
            }
            catch (Exception ex)
            {
                InvokeConnectionStatusChanged($"Pipe cleanup hatası: {ex.Message}");
            }
        }

        private async Task ListenForClientsAsync(CancellationToken cancellationToken)
        {
            while (_isServerRunning && !cancellationToken.IsCancellationRequested)
            {
                try
                {
                    InvokeConnectionStatusChanged("Client bağlantısı bekleniyor...");
                    
                    await _pipeServer.WaitForConnectionAsync(cancellationToken);
                    
                    if (_pipeServer.IsConnected)
                    {
                        _isClientConnected = true;
                        InvokeConnectionStatusChanged("Client bağlandı!");
                        
                        // Hoş geldin mesajı gönder
                        await SendMessageToClientAsync("Access granted. Welcome to the LUCID central system.");
                        
                        // Client bağlantısını sürdür (sadece gönderim için)
                        await MaintainClientConnectionAsync(cancellationToken);
                        
                        _isClientConnected = false;
                        InvokeConnectionStatusChanged("Client bağlantısı kesildi.");
                        _pipeServer.Disconnect();
                        
                        if (_isServerRunning && !cancellationToken.IsCancellationRequested)
                        {
                            _pipeServer.Dispose();
                            _pipeServer = new NamedPipeServerStream(
                                PipeName,
                                PipeDirection.Out,  // Sadece gönderme için
                                1,
                                PipeTransmissionMode.Byte,
                                PipeOptions.Asynchronous);
                        }
                    }
                }
                catch (OperationCanceledException)
                {
                    InvokeConnectionStatusChanged("Server durduruldu.");
                    break;
                }
                catch (Exception ex)
                {
                    InvokeConnectionStatusChanged($"Client dinleme hatası: {ex.Message}");
                    await Task.Delay(1000, cancellationToken);
                }
            }
        }

        private async Task MaintainClientConnectionAsync(CancellationToken cancellationToken)
        {
            try
            {
                // Client bağlantısını sürdür, sadece gönderim için hazır bekle
                while (_pipeServer.IsConnected && !cancellationToken.IsCancellationRequested)
                {
                    // Bağlantı durumunu kontrol et
                    await Task.Delay(1000, cancellationToken);
                    
                    // Pipe'ın hala bağlı olup olmadığını kontrol et
                    if (!_pipeServer.IsConnected)
                    {
                        InvokeConnectionStatusChanged("Client bağlantısı kesildi.");
                        break;
                    }
                }
            }
            catch (OperationCanceledException)
            {
                InvokeConnectionStatusChanged("Client bağlantısı iptal edildi.");
            }
            catch (Exception ex)
            {
                InvokeConnectionStatusChanged($"Client bağlantı sürdürme hatası: {ex.Message}");
            }
        }
        
        private async Task SendMessageAsync(string message, CancellationToken cancellationToken)
        {
            try
            {
                byte[] messageBytes = Encoding.Unicode.GetBytes(message + '\0');
                await _pipeServer.WriteAsync(messageBytes, 0, messageBytes.Length, cancellationToken);
                await _pipeServer.FlushAsync(cancellationToken);
            }
            catch (Exception ex)
            {
                InvokeConnectionStatusChanged($"Mesaj gönderme hatası: {ex.Message}");
                throw;
            }
        }
        
        public async Task<bool> SendMessageToClientAsync(string message)
        {
            if (!_isClientConnected || !_pipeServer?.IsConnected == true)
            {
                InvokeConnectionStatusChanged("Client bağlı değil, mesaj gönderilemez.");
                return false;
            }

            try
            {
                await SendMessageAsync(message, _serverCancellationToken.Token);
                InvokeConnectionStatusChanged($"Client'a mesaj gönderildi: {message}");
                return true;
            }
            catch (Exception ex)
            {
                InvokeConnectionStatusChanged($"Client'a mesaj gönderme hatası: {ex.Message}");
                return false;
            }
        }
        
        public async Task StopServerAsync()
        {
            InvokeConnectionStatusChanged("Server durduruluyor...");
            _isServerRunning = false;
            
            try
            {
                _serverCancellationToken?.Cancel();
                
                await Task.Delay(500);
                
                if (_pipeServer != null)
                {
                    try
                    {
                        if (_pipeServer.IsConnected)
                        {
                            InvokeConnectionStatusChanged("Client bağlantısı kesiliyor...");
                            _pipeServer.Disconnect();
                        }
                    }
                    catch (Exception ex)
                    {
                        InvokeConnectionStatusChanged($"Pipe disconnect hatası: {ex.Message}");
                    }
                    
                    try
                    {
                        _pipeServer.Dispose();
                        _pipeServer = null;
                        InvokeConnectionStatusChanged("Pipe kaynakları temizlendi.");
                    }
                    catch (Exception ex)
                    {
                        InvokeConnectionStatusChanged($"Pipe dispose hatası: {ex.Message}");
                    }
                }
                
                try
                {
                    _serverCancellationToken?.Dispose();
                    _serverCancellationToken = null;
                }
                catch { }
                
                _isClientConnected = false;
                
                GC.Collect();
                GC.WaitForPendingFinalizers();
                
                InvokeConnectionStatusChanged("Server başarıyla durduruldu ve tüm kaynaklar temizlendi.");
            }
            catch (Exception ex)
            {
                InvokeConnectionStatusChanged($"Server durdurma hatası: {ex.Message}");
            }
        }
        
        public void StopServer()
        {
            _ = Task.Run(StopServerAsync);
        }

        public void Dispose()
        {
            StopServer();
            _serverSemaphore?.Dispose();
            _serverCancellationToken?.Dispose();
        }
    }
}