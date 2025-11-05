using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Platform.Storage;

namespace ArabicIDE.CrossPlatform.Services;

/// <summary>
/// Service for file I/O operations using Avalonia's StorageProvider
/// </summary>
public class FileService : IFileService
{
    private static Window? GetMainWindow()
    {
        if (Application.Current?.ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
        {
            return desktop.MainWindow;
        }
        return null;
    }

    public async Task<string?> OpenFileAsync()
    {
        var window = GetMainWindow();
        if (window == null) return null;

        var files = await window.StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "فتح ملف برنامج عربي (Open Arabic Program)",
            AllowMultiple = false,
            FileTypeFilter = new[]
            {
                new FilePickerFileType("Arabic Programs")
                {
                    Patterns = new[] { "*.arabic" }
                },
                FilePickerFileTypes.All
            }
        });

        return files.FirstOrDefault()?.Path.LocalPath;
    }

    public async Task<string?> SaveFileAsync(string content, string? currentFilePath = null)
    {
        // If we have a current file path, just save to it
        if (!string.IsNullOrEmpty(currentFilePath))
        {
            await File.WriteAllTextAsync(currentFilePath, content, Encoding.UTF8);
            return currentFilePath;
        }

        // Otherwise, prompt for location
        return await SaveFileAsAsync(content);
    }

    public async Task<string?> SaveFileAsAsync(string content)
    {
        var window = GetMainWindow();
        if (window == null) return null;

        var file = await window.StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
        {
            Title = "حفظ الملف باسم (Save File As)",
            DefaultExtension = "arabic",
            SuggestedFileName = "غير_مسمى.arabic",
            FileTypeChoices = new[]
            {
                new FilePickerFileType("Arabic Programs")
                {
                    Patterns = new[] { "*.arabic" }
                },
                FilePickerFileTypes.All
            }
        });

        if (file != null)
        {
            var filePath = file.Path.LocalPath;
            await File.WriteAllTextAsync(filePath, content, Encoding.UTF8);
            return filePath;
        }

        return null;
    }

    public async Task<string> ReadFileAsync(string filePath)
    {
        return await File.ReadAllTextAsync(filePath, Encoding.UTF8);
    }
}

