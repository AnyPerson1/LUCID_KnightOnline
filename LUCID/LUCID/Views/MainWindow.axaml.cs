using System;
using Avalonia.Controls;

namespace LUCID.Views
{
    public partial class MainWindow : Window
    {
        private MacroPageControl _macroPageControl;
        private Action
        public MainWindow()
        {
            InitializeComponent();
            _macroPageControl = new MacroPageControl();
            MacroTabControlContainer.Content = _macroPageControl;
        }
        
        
    }
}