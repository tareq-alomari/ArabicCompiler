using System;
using System.Text;
using System.Threading.Tasks;
using ArabicIDE.CrossPlatform.Services;
using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace ArabicIDE.CrossPlatform.ViewModels;

public partial class MainWindowViewModel : ViewModelBase
{
    private readonly ICompilerService _compilerService;
    private readonly IFileService _fileService;

    [ObservableProperty]
    private string _editorText = "برنامج اختبار ؛\nاطبع \"مرحبا من المترجم العربي!\" ؛\n.";

    [ObservableProperty]
    private string _outputText = "";

    [ObservableProperty]
    private string _statusText = "جاهز (Ready)";

    [ObservableProperty]
    private string _currentFileName = "غير_مسمى.arabic";

    [ObservableProperty]
    [NotifyCanExecuteChangedFor(nameof(RunCommand))]
    private bool _canRun = false;

    private string? _currentFilePath;
    private string? _lastGeneratedCFile;
    private bool _hasUnsavedChanges;

    public MainWindowViewModel()
    {
        _compilerService = new CompilerService();
        _fileService = new FileService();
        
        // Monitor text changes for unsaved changes indicator
        PropertyChanged += (s, e) =>
        {
            if (e.PropertyName == nameof(EditorText))
            {
                _hasUnsavedChanges = true;
                UpdateFileName();
            }
        };
    }

    [RelayCommand]
    private void NewFile()
    {
        if (_hasUnsavedChanges)
        {
            // TODO: Add confirmation dialog in future
        }

        EditorText = "برنامج جديد ؛\n\n.\n";
        OutputText = "";
        _currentFilePath = null;
        _hasUnsavedChanges = false;
        CurrentFileName = "غير_مسمى.arabic";
        StatusText = "ملف جديد (New file)";
    }

    [RelayCommand]
    private async Task OpenFile()
    {
        var filePath = await _fileService.OpenFileAsync();
        if (filePath == null) return;

        try
        {
            EditorText = await _fileService.ReadFileAsync(filePath);
            _currentFilePath = filePath;
            _hasUnsavedChanges = false;
            CurrentFileName = System.IO.Path.GetFileName(filePath);
            StatusText = $"فتح: {CurrentFileName}";
            OutputText += $"\n📂 تم فتح الملف: {CurrentFileName}\n";
        }
        catch (Exception ex)
        {
            OutputText += $"\n❌ خطأ في فتح الملف: {ex.Message}\n";
            StatusText = "خطأ في فتح الملف (Error opening file)";
        }
    }

    [RelayCommand]
    private async Task SaveFile()
    {
        try
        {
            var savedPath = await _fileService.SaveFileAsync(EditorText, _currentFilePath);
            if (savedPath != null)
            {
                _currentFilePath = savedPath;
                _hasUnsavedChanges = false;
                CurrentFileName = System.IO.Path.GetFileName(savedPath);
                StatusText = $"تم الحفظ: {CurrentFileName}";
                OutputText += $"\n💾 تم حفظ الملف: {CurrentFileName}\n";
            }
        }
        catch (Exception ex)
        {
            OutputText += $"\n❌ خطأ في حفظ الملف: {ex.Message}\n";
            StatusText = "خطأ في الحفظ (Error saving)";
        }
    }

    [RelayCommand]
    private async Task SaveFileAs()
    {
        try
        {
            var savedPath = await _fileService.SaveFileAsAsync(EditorText);
            if (savedPath != null)
            {
                _currentFilePath = savedPath;
                _hasUnsavedChanges = false;
                CurrentFileName = System.IO.Path.GetFileName(savedPath);
                StatusText = $"تم الحفظ باسم: {CurrentFileName}";
                OutputText += $"\n💾 تم حفظ الملف باسم: {CurrentFileName}\n";
            }
        }
        catch (Exception ex)
        {
            OutputText += $"\n❌ خطأ في حفظ الملف: {ex.Message}\n";
            StatusText = "خطأ في الحفظ (Error saving)";
        }
    }

    [RelayCommand]
    private async Task Compile()
    {
        StatusText = "⏳ جاري الترجمة... (Compiling...)";
        CanRun = false;
        
        var resultBuilder = new StringBuilder();
        resultBuilder.AppendLine("\n╔══════════════════════════════════════════════════════════════╗");
        resultBuilder.AppendLine("║          بدء الترجمة (Starting Compilation)                ║");
        resultBuilder.AppendLine("╚══════════════════════════════════════════════════════════════╝\n");

        var result = await _compilerService.CompileAsync(EditorText);

        resultBuilder.AppendLine("=== مخرجات المترجم (Compiler Output) ===");
        resultBuilder.AppendLine(result.Output);

        if (!result.Success)
        {
            if (!string.IsNullOrEmpty(result.ErrorOutput))
            {
                resultBuilder.AppendLine("\n=== أخطاء (Errors) ===");
                resultBuilder.AppendLine(result.ErrorOutput);
            }
            resultBuilder.AppendLine("\n❌ فشلت الترجمة! (Compilation failed!)");
            StatusText = "❌ فشلت الترجمة (Compilation failed)";
        }
        else
        {
            resultBuilder.AppendLine("\n✅ نجحت الترجمة! (Compilation successful!)");
            StatusText = "✅ نجحت الترجمة (Compilation successful)";
            _lastGeneratedCFile = result.GeneratedCFilePath;
            CanRun = true;
        }

        OutputText += resultBuilder.ToString();
    }

    private bool CanExecuteRun() => CanRun;

    [RelayCommand(CanExecute = nameof(CanExecuteRun))]
    private async Task Run()
    {
        if (_lastGeneratedCFile == null) return;

        StatusText = "▶️ جاري التشغيل... (Running...)";

        var resultBuilder = new StringBuilder(OutputText);
        resultBuilder.AppendLine("\n╔══════════════════════════════════════════════════════════════╗");
        resultBuilder.AppendLine("║           تشغيل البرنامج (Running Program)                 ║");
        resultBuilder.AppendLine("╚══════════════════════════════════════════════════════════════╝\n");

        var result = await _compilerService.CompileAndRunAsync(_lastGeneratedCFile);

        if (!result.Success)
        {
            resultBuilder.AppendLine("❌ فشل التشغيل! (Execution failed!)");
            if (!string.IsNullOrEmpty(result.ErrorOutput))
            {
                resultBuilder.AppendLine("\nأخطاء (Errors):");
                resultBuilder.AppendLine(result.ErrorOutput);
            }
            StatusText = "❌ فشل التشغيل (Execution failed)";
        }
        else
        {
            resultBuilder.AppendLine("📤 مخرجات البرنامج (Program Output):");
            resultBuilder.AppendLine("─────────────────────────────────────────────────────────────");
            resultBuilder.AppendLine(result.Output.Trim());
            resultBuilder.AppendLine("─────────────────────────────────────────────────────────────");
            resultBuilder.AppendLine("\n✅ تم التشغيل بنجاح! (Execution successful!)");
            StatusText = "✅ تم التشغيل بنجاح (Execution successful)";
        }

        OutputText = resultBuilder.ToString();
    }

    [RelayCommand]
    private void ClearOutput()
    {
        OutputText = "";
        StatusText = "تم مسح المخرجات (Output cleared)";
    }

    [RelayCommand]
    private void Exit()
    {
        if (Application.Current?.ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
        {
            desktop.Shutdown();
        }
    }

    private void UpdateFileName()
    {
        if (string.IsNullOrEmpty(_currentFilePath))
        {
            CurrentFileName = _hasUnsavedChanges ? "* غير_مسمى.arabic" : "غير_مسمى.arabic";
        }
        else
        {
            var fileName = System.IO.Path.GetFileName(_currentFilePath);
            CurrentFileName = _hasUnsavedChanges ? $"* {fileName}" : fileName;
        }
    }
}
