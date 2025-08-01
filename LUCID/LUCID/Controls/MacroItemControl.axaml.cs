using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Markup.Xaml;
using Avalonia.ReactiveUI;
using LUCID.ViewModels;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;

namespace LUCID.Controls;

public partial class MacroItemControl : UserControl
{

    public MacroItemControl()
    {
        InitializeComponent();
        DataContext = new MacroItemViewModel();
    }
}