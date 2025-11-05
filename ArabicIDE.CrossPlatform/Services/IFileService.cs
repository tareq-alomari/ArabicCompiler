using System.Threading.Tasks;

namespace ArabicIDE.CrossPlatform.Services;

/// <summary>
/// Service interface for file operations
/// </summary>
public interface IFileService
{
    /// <summary>
    /// Opens a file dialog and returns the selected file path
    /// </summary>
    Task<string?> OpenFileAsync();

    /// <summary>
    /// Saves content to a file, prompting for location if path is null
    /// </summary>
    Task<string?> SaveFileAsync(string content, string? currentFilePath = null);

    /// <summary>
    /// Always prompts for save location and saves the content
    /// </summary>
    Task<string?> SaveFileAsAsync(string content);

    /// <summary>
    /// Reads the content of a file
    /// </summary>
    Task<string> ReadFileAsync(string filePath);
}

