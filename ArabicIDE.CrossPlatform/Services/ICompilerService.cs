using System.Threading.Tasks;

namespace ArabicIDE.CrossPlatform.Services;

/// <summary>
/// Service interface for interacting with the Arabic compiler
/// </summary>
public interface ICompilerService
{
    /// <summary>
    /// Compiles Arabic source code to C code
    /// </summary>
    /// <param name="sourceCode">The Arabic source code</param>
    /// <returns>Result containing success status, output, and generated C file path</returns>
    Task<CompilationResult> CompileAsync(string sourceCode);

    /// <summary>
    /// Compiles the generated C code to an executable and runs it
    /// </summary>
    /// <param name="cFilePath">Path to the C file to compile and run</param>
    /// <returns>Result containing the program's output</returns>
    Task<ExecutionResult> CompileAndRunAsync(string cFilePath);
}

/// <summary>
/// Result of a compilation operation
/// </summary>
public record CompilationResult(
    bool Success,
    string Output,
    string? GeneratedCFilePath = null,
    string? ErrorOutput = null
);

/// <summary>
/// Result of an execution operation
/// </summary>
public record ExecutionResult(
    bool Success,
    string Output,
    string? ErrorOutput = null
);

