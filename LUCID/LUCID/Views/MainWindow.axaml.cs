using System;
using System.Diagnostics;
using System.Threading;
using Avalonia.Controls;
using Avalonia.Interactivity;
using LUCID.Models;
using LUCID.ViewModels;

namespace LUCID.Views
{
    public partial class MainWindow : Window
    {
        private MacroPageControl _macroPageControl;
        private Action<MacroItem> _addMacroToList;
        private MainWindowViewModel _mainWindowViewModel;
        Stopwatch sw = new Stopwatch();
        
        public MainWindow()
        {
            InitializeComponent();
            _macroPageControl = new MacroPageControl();
            MacroTabControlContainer.Content = _macroPageControl;
            _addMacroToList += _macroPageControl.AddMacroToList;
            _mainWindowViewModel = new MainWindowViewModel();
            _mainWindowViewModel.ConnectionStatusChanged += updatestatus;
            this.Loaded += StartServer;
        }

        private async void StartServer(object? sender, RoutedEventArgs e)
        {
            sw.Start();
            
            // Named Pipe Server'ı başlat
            await _mainWindowViewModel.CreatePipeServer();
            
            sw.Stop();
            infotext.Text = $"Server started in {sw.ElapsedMilliseconds}ms. Waiting for C++ client...";
        }

        public void updateinfo(string message)
        {
            infotext.Text = $"Last message: {message} (at {DateTime.Now:HH:mm:ss})";
        }

        public void updatestatus(string message)
        {
            infotext2.Text += $"\n{DateTime.Now:HH:mm:ss} - {message}";
            
            // Scroll to bottom (optional)
            // You might need to implement scrolling logic here
        }

        // Window kapatılırken server'ı durdur
        protected override async void OnClosing(WindowClosingEventArgs e)
        {
            if (_mainWindowViewModel != null)
            {
                // UI'ı hemen kapatma, server'ın düzgün durmasını bekle
                e.Cancel = true;
                
                infotext.Text = "Uygulama kapatılıyor, server durduruluyor...";
                
                try
                {
                    await _mainWindowViewModel.StopServerAsync();
                    _mainWindowViewModel.Dispose();
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Shutdown error: {ex.Message}");
                }
                
                // Şimdi gerçekten kapat
                e.Cancel = false;
                base.OnClosing(e);
                
                // Uygulamayı zorla kapat
                Environment.Exit(0);
            }
            else
            {
                base.OnClosing(e);
            }
        }

        // Test için server'dan client'a mesaj gönderme
        private async void SendTestMessage(object? sender, RoutedEventArgs e)
        {
            if (_mainWindowViewModel != null)
            {
                bool success = await _mainWindowViewModel.SendMessageToClientAsync("TEST_MESSAGE_FROM_SERVER");
                infotext.Text = success ? "Test message sent!" : "Failed to send test message.";
            }
        }
    }
}