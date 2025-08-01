namespace LUCID.Models;


[System.Serializable]
public struct MacroItem
{
    public bool isActive { get; set; }
    public string macroHeader { get; set; }
    public string macroDescription { get; set; }
    public int macroSpeed { get; set; }
    public string macroHotkey { get; set; }
    public Tab macroTab { get; set; }
}