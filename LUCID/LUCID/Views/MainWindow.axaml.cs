using System;
using Avalonia.Controls;
using LUCID.Models;

namespace LUCID.Views
{
    public partial class MainWindow : Window
    {
        private MacroPageControl _macroPageControl;
        private Action<MacroItem> _addMacroToList;
        public MainWindow()
        {
            InitializeComponent();
            _macroPageControl = new MacroPageControl();
            MacroTabControlContainer.Content = _macroPageControl;
            _addMacroToList += _macroPageControl.AddMacroToList;

            this.Loaded += test;
        }

        private void test(object sender, Avalonia.Interactivity.RoutedEventArgs e)
        {
            _addMacroToList.Invoke(new MacroItem
            {
                isActive = true,
                macroHeader = "Struct & Invoke test",
                macroDescription = "Struct & Invoke test",
                macroSpeed = 100,
                macroHotkey = "Null",
                macroTab = Tab.WarriorMacro
            });
        }
        
        
    }
}