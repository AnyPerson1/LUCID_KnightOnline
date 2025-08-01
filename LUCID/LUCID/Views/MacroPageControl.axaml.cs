using System.Collections.ObjectModel;
using System.Linq.Expressions;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using LUCID.Controls;
using LUCID.Models;
using LUCID.ViewModels;
using ReactiveUI.Fody.Helpers;

namespace LUCID.Views;

public partial class MacroPageControl : UserControl
{
    [Reactive] private ObservableCollection<MacroItemViewModel> Macros { get; set; }

    public MacroPageControl()
    {
        Macros = new ObservableCollection<MacroItemViewModel>();
        InitializeComponent();
        
        
    }
    public void AddMacroToList(MacroItem data)
    {
        var vm = new MacroItemViewModel
        {
            isActive = data.isActive,
            macroHeader = data.macroHeader,
            macroDescription = data.macroDescription,
            macroSpeed = data.macroSpeed,
            macroHotkey = data.macroHotkey,
            macroTab = data.macroTab
        };
        var nmc = new MacroItemControl();
        nmc.DataContext = vm;
            
        Macros.Add(vm);

        switch ((int)vm.macroTab)
        {
            case 0:
                KalkanMacro.Children.Add(nmc);
                break;
            case 1:
                WarriorMacro.Children.Add(nmc);
                break;
            case 2:
                AssassinMacro.Children.Add(nmc);
                break;
            case 3:
                ArcheryMacro.Children.Add(nmc);
                break;
            case 4:
                PriestMacro.Children.Add(nmc);
                break;
            case 5:
                AutoPotMacro.Children.Add(nmc);
                break;
            case 6:
                UpgradeMacro.Children.Add(nmc);
                break;
            case 7:
                FarmMacro.Children.Add(nmc);
                break;
        }
    }
}