using ReactiveUI;
using ReactiveUI.Fody.Helpers;

namespace LUCID.ViewModels
{
    public partial class MainWindowViewModel : ReactiveObject
    {
        [Reactive] public string Greeting { get; set; } = "Welcome to Avalonia!";
    }
}
