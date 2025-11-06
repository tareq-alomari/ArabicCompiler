using Avalonia.Controls;
using AvaloniaEdit;
using ArabicIDE.CrossPlatform.ViewModels;
using System;

namespace ArabicIDE.CrossPlatform.Views;

public partial class MainWindow : Window
{
    private TextEditor? _textEditor;
    private bool _isUpdatingFromViewModel;
    private bool _isUpdatingFromEditor;

    public MainWindow()
    {
        InitializeComponent();
        
        // Set up TextEditor binding after initialization
        this.Opened += OnWindowOpened;
    }

    private void OnWindowOpened(object? sender, EventArgs e)
    {
        _textEditor = this.FindControl<TextEditor>("TextEditor");
        
        if (_textEditor != null && DataContext is MainWindowViewModel viewModel)
        {
            // Initialize editor content from ViewModel
            // NO custom FlowDirection manipulation - let AvaloniaEdit handle Arabic text naturally
            _textEditor.Text = viewModel.EditorText;

            // Subscribe to editor text changes
            _textEditor.TextChanged += (s, args) =>
            {
                if (!_isUpdatingFromViewModel && _textEditor != null)
                {
                    _isUpdatingFromEditor = true;
                    viewModel.EditorText = _textEditor.Text;
                    _isUpdatingFromEditor = false;
                }
            };
            
            // Subscribe to ViewModel property changes
            viewModel.PropertyChanged += (s, args) =>
            {
                if (args.PropertyName == nameof(MainWindowViewModel.EditorText) && 
                    !_isUpdatingFromEditor && 
                    _textEditor != null)
                {
                    _isUpdatingFromViewModel = true;
                    if (_textEditor.Text != viewModel.EditorText)
                    {
                        _textEditor.Text = viewModel.EditorText;
                    }
                    _isUpdatingFromViewModel = false;
                }
            };
        }
    }
}