using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace ArabicIDE.Avalonia.ViewModels;

public partial class MainWindowViewModel : ViewModelBase
{
    [ObservableProperty]
    private string _codeText = "برنامج اختبار ؛\nاطبع \"مرحبا من Avalonia!\"؛\n.";

    [ObservableProperty]
    private string _outputText = "مخرجات المترجم ستظهر هنا... (Compiler output will appear here...)";

    [ObservableProperty]
    [NotifyCanExecuteChangedFor(nameof(RunCommand))]
    private bool _canRun = false;

    private string _lastGeneratedCFile = string.Empty;
    private string _lastExecutableFile = string.Empty;


    [RelayCommand]
    private async Task Compile()
    {
        OutputText = "⏳ جاري الترجمة...";
        CanRun = false; // Disable Run button during compilation

        // Try multiple paths to find the compiler
        string[] compilerCandidates = new[]
        {
            "../../../../../Compiler/build/ArabicCompiler", // From bin/Debug/net9.0/
            "/Users/salahmac/Desktop/MLProjects/ArabicCompiler/Compiler/build/ArabicCompiler", // Absolute path
            "../../../../Compiler/build/ArabicCompiler" // Backup
        };

        string? compilerPath = null;
        foreach (var candidate in compilerCandidates)
        {
            if (File.Exists(candidate))
            {
                compilerPath = candidate;
                break;
            }
        }
        
        if (compilerPath == null)
        {
            OutputText = $"❌ خطأ: المترجم غير موجود في أي من المسارات المتوقعة:\n";
            foreach (var candidate in compilerCandidates)
            {
                OutputText += $"  - {Path.GetFullPath(candidate)}\n";
            }
            OutputText += "\nPlease build the C++ compiler first using 'Compiler/build.sh'";
            return;
        }

        string tempSourceFile = Path.ChangeExtension(Path.GetTempFileName(), ".arabic");
        
        try
        {
            await File.WriteAllTextAsync(tempSourceFile, CodeText, Encoding.UTF8);

            using var process = new Process();
            process.StartInfo = new ProcessStartInfo
            {
                FileName = compilerPath,
                Arguments = $"\"{tempSourceFile}\" --c",
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                StandardOutputEncoding = Encoding.UTF8,
                StandardErrorEncoding = Encoding.UTF8
            };

            process.Start();
            
            string output = await process.StandardOutput.ReadToEndAsync();
            string error = await process.StandardError.ReadToEndAsync();
            
            await process.WaitForExitAsync();

            var resultBuilder = new StringBuilder();
            resultBuilder.AppendLine("=== COMPILER OUTPUT ===");
            resultBuilder.AppendLine(output);

            if (!string.IsNullOrWhiteSpace(error))
            {
                resultBuilder.AppendLine("\n=== ERRORS ===");
                resultBuilder.AppendLine(error);
                OutputText = resultBuilder.ToString();
                return; // Stop if compilation failed
            }
            
            OutputText = resultBuilder.ToString();
            
            // If compilation was successful, prepare for the Run step
            string baseName = Path.GetFileNameWithoutExtension(tempSourceFile);
            _lastGeneratedCFile = Path.Combine(Path.GetTempPath(), $"{baseName}_output.c");
            _lastExecutableFile = Path.Combine(Path.GetTempPath(), baseName);

            if (File.Exists(_lastGeneratedCFile))
            {
                CanRun = true;
            }
        }
        catch (System.Exception ex)
        {
            OutputText = $"💥 EXCEPTION: {ex.Message}";
        }
    }

    private bool CanExecuteRun() => CanRun;

    [RelayCommand(CanExecute = nameof(CanExecuteRun))]
    private async Task Run()
    {
        var resultBuilder = new StringBuilder(OutputText);
        resultBuilder.AppendLine("\n\n=== RUNNING PROGRAM ===");
        
        // Step 1: Compile the C code with gcc
        try
        {
            using var gccProcess = new Process();
            gccProcess.StartInfo = new ProcessStartInfo
            {
                FileName = "gcc",
                Arguments = $"\"{_lastGeneratedCFile}\" -o \"{_lastExecutableFile}\"",
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true
            };
            gccProcess.Start();
            string gccError = await gccProcess.StandardError.ReadToEndAsync();
            await gccProcess.WaitForExitAsync();

            if (gccProcess.ExitCode != 0)
            {
                resultBuilder.AppendLine("GCC Compilation Failed:");
                resultBuilder.AppendLine(gccError);
                OutputText = resultBuilder.ToString();
                return;
            }
        }
        catch (System.ComponentModel.Win32Exception)
        {
             resultBuilder.AppendLine("❌ ERROR: 'gcc' command not found. Is it installed and in your PATH?");
             OutputText = resultBuilder.ToString();
             return;
        }


        // Step 2: Run the compiled executable
        using var runProcess = new Process();
        runProcess.StartInfo = new ProcessStartInfo
        {
            FileName = _lastExecutableFile,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true,
            StandardOutputEncoding = Encoding.UTF8
        };
        runProcess.Start();
        string programOutput = await runProcess.StandardOutput.ReadToEndAsync();
        await runProcess.WaitForExitAsync();

        resultBuilder.AppendLine("Program Output:");
        resultBuilder.AppendLine(programOutput.Trim());
        OutputText = resultBuilder.ToString();
    }
}
