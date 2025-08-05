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
        }
        
        protected override async void OnClosing(WindowClosingEventArgs e)
        {
            if (_mainWindowViewModel != null)
            {
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
                
                e.Cancel = false;
                base.OnClosing(e);
                
                Environment.Exit(0);
            }
            else
            {
                base.OnClosing(e);
            }
        }
    }
}