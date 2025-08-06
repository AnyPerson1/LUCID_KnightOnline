using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using LUCID.Controls;
using LUCID.Models;
using System.IO;
using System.IO.Pipes;
using System.Text;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;
using System.Threading.Tasks;
using System.Threading;
using Avalonia;
using Avalonia.Threading;

namespace LUCID.ViewModels
{
    public partial class MainWindowViewModel : ReactiveObject
    {
        private const string PIPE_NAME = "LUCID_MacroPipe";
        private const int MAX_RETRY_ATTEMPTS = 5;
        private const int RETRY_DELAY_MS = 1000;
        private const int CONNECTION_CHECK_INTERVAL_MS = 1000;
        private const int SHUTDOWN_DELAY_MS = 500;

        private Dictionary<M_COMMAND, string> commands = new Dictionary<M_COMMAND, string>
        {
            { M_COMMAND.ACTIVATE, "MACRO_ACTIVATE:" },
            { M_COMMAND.DEACTIVATE, "MACRO_DEACTIVATE:" },
            { M_COMMAND.INFO, "MACRO_INFO:" },
            { M_COMMAND.MESSAGE, "MESSAGE:" },
            { M_COMMAND.PANIC, "PANIC:" }
        };
        
        private NamedPipeServerStream _pipeServer;
        private readonly SemaphoreSlim _serverSemaphore = new SemaphoreSlim(1, 1);
        private volatile bool _isServerRunning = false;
        private volatile bool _isClientConnected = false;
        private CancellationTokenSource _serverCancellationToken;
        
        public event Action<string> ConnectionStatusChanged;

        private void LogStatus(string status, string message)
        {
            string timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
            string logMessage = $"[{status}] {timestamp} - {message}";
            
            Console.WriteLine(logMessage);
            
            try
            {
                Dispatcher.UIThread.InvokeAsync(() => ConnectionStatusChanged?.Invoke(logMessage));
            }
            catch (Exception ex)
            {
                Console.WriteLine($"[UI_ERROR] {timestamp} - Event dispatch failed: {ex.Message}");
            }
        }

        public async Task CreatePipeServer()
        {
            await _serverSemaphore.WaitAsync();
            try
            {
                if (_isServerRunning)
                {
                    LogStatus("WARNING", "Server already running");
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
                await CleanupPreviousInstance();
                LogStatus("INIT", "Starting Named Pipe Server...");

                for (int attempt = 1; attempt <= MAX_RETRY_ATTEMPTS; attempt++)
                {
                    try
                    {
                        LogStatus("ATTEMPT", $"Server creation attempt {attempt}/{MAX_RETRY_ATTEMPTS}");
                        
                        _pipeServer = new NamedPipeServerStream(
                            PIPE_NAME,
                            PipeDirection.Out,
                            1,
                            PipeTransmissionMode.Byte,
                            PipeOptions.Asynchronous,
                            512, 
                            0     
                        );

                        _isServerRunning = true;
                        LogStatus("SUCCESS", $"Server created successfully: {PIPE_NAME}");
                        break;
                    }
                    catch (IOException ex) when (ex.Message.Contains("busy") || ex.Message.Contains("meşgul"))
                    {
                        LogStatus("BUSY", $"Pipe busy (attempt {attempt}/{MAX_RETRY_ATTEMPTS}), waiting {RETRY_DELAY_MS}ms...");
                        await Task.Delay(RETRY_DELAY_MS);
                        
                        if (attempt == MAX_RETRY_ATTEMPTS)
                        {
                            LogStatus("TIMEOUT", "Maximum retry attempts reached. Cleaning system pipes...");
                            await CleanupSystemPipes();
                            throw;
                        }
                    }
                    catch (Exception ex)
                    {
                        LogStatus("ERROR", $"Server creation failed (attempt {attempt}): {ex.Message}");
                        if (attempt == MAX_RETRY_ATTEMPTS)
                            throw;
                        await Task.Delay(RETRY_DELAY_MS);
                    }
                }
                
                if (_isServerRunning && _pipeServer != null)
                {
                    _ = Task.Run(() => ListenForClientsAsync(_serverCancellationToken.Token));
                }
            }
            catch (Exception ex)
            {
                LogStatus("CRITICAL_ERROR", $"Server startup failed: {ex.Message}");
                _isServerRunning = false;
                await DisposeServerInstance();
            }
        }

        private async Task CleanupPreviousInstance()
        {
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
        }

        private async Task CleanupSystemPipes()
        {
            try
            {
                LogStatus("CLEANUP", "Cleaning system pipes...");
                await Task.Delay(2000);
                GC.Collect();
                GC.WaitForPendingFinalizers();
                LogStatus("CLEANUP_SUCCESS", "Pipe cleanup completed");
            }
            catch (Exception ex)
            {
                LogStatus("CLEANUP_ERROR", $"Pipe cleanup failed: {ex.Message}");
            }
        }

        private async Task ListenForClientsAsync(CancellationToken cancellationToken)
        {
            while (_isServerRunning && !cancellationToken.IsCancellationRequested)
            {
                try
                {
                    LogStatus("WAITING", "Awaiting client connection...");
                    
                    await _pipeServer.WaitForConnectionAsync(cancellationToken);
                    
                    if (_pipeServer.IsConnected)
                    {
                        _isClientConnected = true;
                        LogStatus("CONNECTED", "Client connected successfully!"); 
 // MACRO ITEM SEND SAMPLE : MACRO_INFO:ID:macroHeader:MS:macroSpeed:CLICK_POINT:clickPosition.x,clickPosition.y:SCAN_RECT:scanArea.x,scanArea.y,scanArea.width,scanArea.height:HOTKEY:macroHotkey
                        await SendMessageToClientAsync("MESSAGE:Access granted. Welcome to LUCID central system.");
                        await SendMessageToClientAsync("MESSAGE:A test of macro import will be made.");
                        await SendMessageToClientAsync(
                            "MACRO_INFO:ID:test:MS:1:CLICK_POINT:10,10:SCAN_RECT:0,0,0,0:HOTKEY:TEST");
                        await MaintainClientConnectionAsync(cancellationToken);
                        
                        _isClientConnected = false;
                        LogStatus("DISCONNECTED", "Client connection terminated");
                        _pipeServer.Disconnect();
                        
                        if (_isServerRunning && !cancellationToken.IsCancellationRequested)
                        {
                            await CreateNewServerInstance();
                        }
                    }
                }
                catch (OperationCanceledException)
                {
                    LogStatus("SHUTDOWN", "Server stopped by cancellation");
                    break;
                }
                catch (Exception ex)
                {
                    LogStatus("LISTEN_ERROR", $"Client listening error: {ex.Message}");
                    await Task.Delay(RETRY_DELAY_MS, cancellationToken);
                }
            }
        }

        private async Task SendMacroDataToCore(string id, int ms, Point clickPoint, Rect scanRect, string hotkey )
        {
            
        }
        
        private async Task SendMacroDataToCore(MacroItemViewModel item)
        {
            
        }

        private async Task CreateNewServerInstance()
        {
            _pipeServer.Dispose();
            _pipeServer = new NamedPipeServerStream(
                PIPE_NAME,
                PipeDirection.Out,
                1,
                PipeTransmissionMode.Byte,
                PipeOptions.Asynchronous);
            
            LogStatus("RESET", "New server instance created");
        }

        private async Task MaintainClientConnectionAsync(CancellationToken cancellationToken)
        {
            try
            {
                while (_pipeServer.IsConnected && !cancellationToken.IsCancellationRequested)
                {
                    await Task.Delay(CONNECTION_CHECK_INTERVAL_MS, cancellationToken);
                    
                    if (!_pipeServer.IsConnected)
                    {
                        LogStatus("CONNECTION_LOST", "Client connection lost");
                        break;
                    }
                }
            }
            catch (OperationCanceledException)
            {
                LogStatus("CONNECTION_CANCELLED", "Client connection cancelled");
            }
            catch (Exception ex)
            {
                LogStatus("CONNECTION_ERROR", $"Connection maintenance error: {ex.Message}");
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
                LogStatus("SEND_ERROR", $"Message send failed: {ex.Message}");
                throw;
            }
        }
        
        public async Task<bool> SendMessageToClientAsync(string message)
        {
            if (!_isClientConnected || !_pipeServer?.IsConnected == true)
            {
                LogStatus("SEND_FAILED", "Client not connected, message cannot be sent");
                return false;
            }

            try
            {
                await SendMessageAsync(message, _serverCancellationToken.Token);
                LogStatus("MESSAGE_SENT", $"Message sent to client: {message}");
                return true;
            }
            catch (Exception ex)
            {
                LogStatus("MESSAGE_ERROR", $"Client message send error: {ex.Message}");
                return false;
            }
        }
        
        public async Task StopServerAsync()
        {
            LogStatus("SHUTDOWN_INIT", "Initiating server shutdown...");
            _isServerRunning = false;
            
            try
            {
                _serverCancellationToken?.Cancel();
                await Task.Delay(SHUTDOWN_DELAY_MS);
                
                await DisconnectClient();
                await DisposeServerInstance();
                await DisposeResources();
                
                _isClientConnected = false;
                
                GC.Collect();
                GC.WaitForPendingFinalizers();
                
                LogStatus("SHUTDOWN_SUCCESS", "Server stopped successfully. All resources cleaned");
            }
            catch (Exception ex)
            {
                LogStatus("SHUTDOWN_ERROR", $"Server shutdown error: {ex.Message}");
            }
        }

        private async Task DisconnectClient()
        {
            if (_pipeServer != null)
            {
                try
                {
                    if (_pipeServer.IsConnected)
                    {
                        LogStatus("DISCONNECT", "Disconnecting client...");
                        _pipeServer.Disconnect();
                    }
                }
                catch (Exception ex)
                {
                    LogStatus("DISCONNECT_ERROR", $"Client disconnect error: {ex.Message}");
                }
            }
        }

        private async Task DisposeServerInstance()
        {
            if (_pipeServer != null)
            {
                try
                {
                    _pipeServer.Dispose();
                    _pipeServer = null;
                    LogStatus("DISPOSED", "Pipe resources disposed");
                }
                catch (Exception ex)
                {
                    LogStatus("DISPOSE_ERROR", $"Pipe dispose error: {ex.Message}");
                }
            }
        }

        private async Task DisposeResources()
        {
            try
            {
                _serverCancellationToken?.Dispose();
                _serverCancellationToken = null;
            }
            catch { }
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