using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace ArabicIDE.CrossPlatform.Services;

/// <summary>
/// Service for compiling and running Arabic programs
/// </summary>
public class CompilerService : ICompilerService
{
    private readonly string _compilerPath;

    public CompilerService()
    {
        // Find the compiler using multiple fallback paths
        _compilerPath = FindCompiler() ?? throw new InvalidOperationException(
            "Arabic compiler not found. Please build the compiler first using 'Compiler/build.sh'");
    }

    public async Task<CompilationResult> CompileAsync(string sourceCode)
    {
        try
        {
            // Create temp file with .arabic extension
            var tempSourceFile = Path.ChangeExtension(Path.GetTempFileName(), ".arabic");
            await File.WriteAllTextAsync(tempSourceFile, sourceCode, Encoding.UTF8);

            // Run the compiler
            using var process = new Process();
            process.StartInfo = new ProcessStartInfo
            {
                FileName = _compilerPath,
                Arguments = $"\"{tempSourceFile}\" --c",
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                StandardOutputEncoding = Encoding.UTF8,
                StandardErrorEncoding = Encoding.UTF8
            };

            process.Start();

            var output = await process.StandardOutput.ReadToEndAsync();
            var error = await process.StandardError.ReadToEndAsync();

            await process.WaitForExitAsync();

            if (process.ExitCode == 0 && string.IsNullOrWhiteSpace(error))
            {
                // Compilation successful - find the generated C file
                var baseName = Path.GetFileNameWithoutExtension(tempSourceFile);
                var cFilePath = Path.Combine(Path.GetTempPath(), $"{baseName}_output.c");

                if (File.Exists(cFilePath))
                {
                    return new CompilationResult(true, output, cFilePath);
                }

                return new CompilationResult(false, output, null, "C file was not generated");
            }

            return new CompilationResult(false, output, null, error);
        }
        catch (Exception ex)
        {
            return new CompilationResult(false, "", null, $"Exception: {ex.Message}");
        }
    }

    public async Task<ExecutionResult> CompileAndRunAsync(string cFilePath)
    {
        try
        {
            var executablePath = Path.Combine(Path.GetTempPath(), Path.GetFileNameWithoutExtension(cFilePath));

            // Step 1: Compile C code with gcc
            using (var gccProcess = new Process())
            {
                gccProcess.StartInfo = new ProcessStartInfo
                {
                    FileName = "gcc",
                    Arguments = $"\"{cFilePath}\" -o \"{executablePath}\"",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                };

                gccProcess.Start();
                var gccError = await gccProcess.StandardError.ReadToEndAsync();
                await gccProcess.WaitForExitAsync();

                if (gccProcess.ExitCode != 0)
                {
                    return new ExecutionResult(false, "", $"GCC compilation failed:\n{gccError}");
                }
            }

            // Step 2: Run the executable
            using (var runProcess = new Process())
            {
                runProcess.StartInfo = new ProcessStartInfo
                {
                    FileName = executablePath,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    StandardOutputEncoding = Encoding.UTF8
                };

                runProcess.Start();
                var programOutput = await runProcess.StandardOutput.ReadToEndAsync();
                var programError = await runProcess.StandardError.ReadToEndAsync();
                await runProcess.WaitForExitAsync();

                if (runProcess.ExitCode == 0)
                {
                    return new ExecutionResult(true, programOutput);
                }

                return new ExecutionResult(false, programOutput, programError);
            }
        }
        catch (System.ComponentModel.Win32Exception)
        {
            return new ExecutionResult(false, "", "gcc not found. Please install gcc and ensure it's in your PATH.");
        }
        catch (Exception ex)
        {
            return new ExecutionResult(false, "", $"Exception: {ex.Message}");
        }
    }

    private static string? FindCompiler()
    {
        // Try multiple paths to find the compiler
        var candidates = new[]
        {
            "../../../../../Compiler/build/ArabicCompiler",
            "/Users/salahmac/Desktop/MLProjects/ArabicCompiler/Compiler/build/ArabicCompiler",
            "../../../../Compiler/build/ArabicCompiler",
            "ArabicCompiler" // Try PATH
        };

        foreach (var candidate in candidates)
        {
            try
            {
                var fullPath = Path.GetFullPath(candidate);
                if (File.Exists(fullPath))
                {
                    return fullPath;
                }
            }
            catch
            {
                // Invalid path, continue
            }
        }

        return null;
    }
}

