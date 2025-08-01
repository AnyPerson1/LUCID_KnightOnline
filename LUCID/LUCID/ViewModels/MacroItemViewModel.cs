using ReactiveUI;
using Avalonia.ReactiveUI;
using LUCID.Models;
using ReactiveUI.Fody.Helpers;

namespace LUCID.ViewModels;

public partial class MacroItemViewModel : ReactiveObject
{
    [Reactive] public bool isActive { get; set; } = false;
    [Reactive] public string macroHeader { get; set; } = "Macro";
    [Reactive] public string macroDescription { get; set; } = "Macro description";
    [Reactive] public int macroSpeed { get; set; } = 100;
    [Reactive] public string macroHotkey { get; set; } = "Null";
    
    public Tab macroTab { get; set; }
}