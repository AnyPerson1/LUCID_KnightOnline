using Avalonia;
using ReactiveUI;
using Avalonia.ReactiveUI;
using LUCID.Models;
using ReactiveUI.Fody.Helpers;

namespace LUCID.ViewModels;

public partial class MacroItemViewModel : ReactiveObject // MACRO ITEM SEND SAMPLE : MACRO_INFO:ID:macroHeader:MS:macroSpeed:CLICK_POINT:clickPosition.x,clickPosition.y:SCAN_RECT:scanArea.x,scanArea.y,scanArea.width,scanArea.height:HOTKEY:macroHotkey
{
    [Reactive] public bool isActive { get; set; } = false;
    [Reactive] public string macroHeader { get; set; } = "Macro";
    [Reactive] public string macroDescription { get; set; } = "Macro description";
    [Reactive] public int? macroSpeed { get; set; } = 100;
    [Reactive] public string macroHotkey { get; set; } = "Null";
    [Reactive] public Point? clickPosition { get; set; }
    [Reactive] public Rect? scanArea { get; set; }

    public Tab macroTab { get; set; }
    public string macro_header = "sample_header";
}