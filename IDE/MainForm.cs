using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Text;
using System.ComponentModel;
using System.Text.RegularExpressions;
using Timer = System.Windows.Forms.Timer;
using System.Text.Json;
using System.Threading.Tasks;
using System.Linq;

namespace ArabicIDE
{
    public partial class MainForm : Form
    {
        // runtime-initialized paths (initialized in constructor)
        private string exeDir = "";
        private string? repoRoot = null;
        private string script = "";
        private ToolStripButton? openButton;
        private ToolStripButton? saveButton;
        private ToolStripButton? saveAsButton;
        private ToolStripButton? newButton;
        private ToolStripButton? compileButton;
        private ToolStripButton? runButton;
        private ToolStripButton? runAllButton;
        private ToolStripButton? debugLexerButton;
        private ComboBox? outputTypeComboBox;
        private RichTextBox? editor;
        private RichTextBox? outputBox;
        private string settingsFile;
        private ToolStripStatusLabel? statusLabel;
        private ToolStripStatusLabel? lineLabel;
        private StatusStrip? statusStrip;

    private string compilerPath = "";
        private string currentFile = "";
        private bool isModified = false;

        private System.Windows.Forms.Timer? syntaxHighlightingTimer; // تم التحديد الصريح للنوع
        private bool isHighlighting = false;

        // Define colors
        private Color keywordColor = Color.Blue;
        private Color identifierColor = Color.Black;
        private Color stringColor = Color.Brown;
        private Color numberColor = Color.Green;
        private Color commentColor = Color.Gray;

        public MainForm()
        {
            // تسجيل ترميز Windows-1256
            Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
            
            InitializeComponent();
            SetupComponents();
            // initialize runtime paths (exeDir, repoRoot, script)
            exeDir = Path.GetDirectoryName(Application.ExecutablePath) ?? Directory.GetCurrentDirectory();

            // Find repository root by walking up until we find a 'scripts' folder (max 6 levels)
            string dir = exeDir;
            for (int i = 0; i < 6; i++)
            {
                string candidate = Path.Combine(dir, "scripts");
                if (Directory.Exists(candidate))
                {
                    repoRoot = Directory.GetParent(candidate)?.FullName; // parent of scripts
                    break;
                }
                var parent = Directory.GetParent(dir);
                if (parent == null) break;
                dir = parent.FullName;
            }
            // fallback to known relative path (project root assumption)
            if (repoRoot == null)
            {
                repoRoot = Path.GetFullPath(Path.Combine(exeDir, "..", "..", ".."));
            }

            script = Path.Combine(repoRoot, "scripts", "run_end_to_end.ps1");
            // settings file path
            settingsFile = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "ide_settings.json");
            // prefer built compiler in common locations
            var candidates = new[] {
                Path.Combine(repoRoot ?? exeDir, "Compiler", "build", "Release", "ArabicCompiler.exe"),
                Path.Combine(repoRoot ?? exeDir, "Compiler", "build", "ArabicCompiler.exe"),
                Path.Combine(repoRoot ?? exeDir, "Compiler", "ArabicCompiler.exe"),
                Path.Combine(exeDir, "ArabicCompiler.exe"),
                Path.Combine(repoRoot ?? exeDir, "ArabicCompiler.exe")
            };
            compilerPath = candidates.FirstOrDefault(p => File.Exists(p)) ?? "ArabicCompiler.exe";
            LoadSettings();
            UpdateTitle();
            SetupSyntaxHighlighting();
        }

        private void InitializeComponent()
        {
            // هذا لمنع التعارض مع التصميم اليدوي
            SuspendLayout();
            ResumeLayout(false);
        }

        private void SetupComponents()
        {
            this.Text = "بيئة التطوير العربية - Arabic IDE";
            this.Size = new Size(1200, 800);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.Font = new Font("Tahoma", 9);
            // تم إزالة السطر التالي لمنع مشكلة الأيقونة
            // this.Icon = SystemIcons.Application;

            SetupToolStrip();
            SetupMainLayout();
            SetupStatusBar();
            SetupShortcuts();
        }

        private void SetupToolStrip()
        {
            var mainToolStrip = new ToolStrip();
            mainToolStrip.Dock = DockStyle.Top;
            mainToolStrip.GripStyle = ToolStripGripStyle.Hidden;
            mainToolStrip.BackColor = SystemColors.Control;
            mainToolStrip.RenderMode = ToolStripRenderMode.System;

            newButton = CreateToolStripButton("جديد", "📄", "Ctrl+N");
            openButton = CreateToolStripButton("فتح", "📂", "Ctrl+O");
            saveButton = CreateToolStripButton("حفظ", "💾", "Ctrl+S");
            saveAsButton = CreateToolStripButton("حفظ كـ", "💾", "Ctrl+Shift+S");

            newButton!.Click += NewButton_Click;
            openButton!.Click += OpenButton_Click;
            saveButton!.Click += SaveButton_Click;
            saveAsButton!.Click += SaveAsButton_Click;

            mainToolStrip.Items.Add(newButton);
            mainToolStrip.Items.Add(openButton);
            mainToolStrip.Items.Add(saveButton);
            mainToolStrip.Items.Add(saveAsButton);
            mainToolStrip.Items.Add(new ToolStripSeparator());

            runButton = CreateToolStripButton("ترجمة وتنفيذ", "▶️", "F5");
            compileButton = CreateToolStripButton("ترجمة فقط", "🔧", "Ctrl+B");

            debugLexerButton = CreateToolStripButton("تشخيص Lexer", "🐞", "");
            // Make it toggleable
            debugLexerButton.CheckOnClick = true;
            debugLexerButton.CheckedChanged += (s, e) => SaveSettings();

            runAllButton = CreateToolStripButton("تشغيل الأمثلة", "📦", "تشغيل جميع الأمثلة والاطلاع على السجلات");
            runAllButton.Click += RunAllExamples_Click;

            var loadExamplesButton = CreateToolStripButton("تحميل مثال", "📚", "تحميل أحد الأمثلة المتاحة");
            loadExamplesButton.Click += LoadExamples_Click;

            runButton!.Click += RunButton_Click;
            compileButton!.Click += CompileButton_Click;

            mainToolStrip.Items.Add(runButton);
            mainToolStrip.Items.Add(compileButton);
            mainToolStrip.Items.Add(debugLexerButton);
            mainToolStrip.Items.Add(runAllButton);
            mainToolStrip.Items.Add(loadExamplesButton);
            mainToolStrip.Items.Add(new ToolStripSeparator());

            mainToolStrip.Items.Add(new ToolStripLabel("نوع الإخراج:"));
            outputTypeComboBox = new ComboBox();
            outputTypeComboBox.Items.AddRange(new object[] { "كود Assembly", "كود C", "كود وسيط", "جميع المخرجات" });
            outputTypeComboBox.SelectedIndex = 0;
            outputTypeComboBox.Width = 120;
            outputTypeComboBox.DropDownStyle = ComboBoxStyle.DropDownList;

            var comboHost = new ToolStripControlHost(outputTypeComboBox);
            mainToolStrip.Items.Add(comboHost);

            this.Controls.Add(mainToolStrip);
        }

        private void SetupMainLayout()
        {
            var splitContainer = new SplitContainer();
            splitContainer.Dock = DockStyle.Fill;
            splitContainer.Orientation = Orientation.Vertical;
            splitContainer.SplitterDistance = 600;
            splitContainer.SplitterWidth = 5;

            // لوحة المحرر
            var editorPanel = new Panel();
            editorPanel.Dock = DockStyle.Fill;
            editorPanel.Padding = new Padding(5);

            var editorLabel = new Label();
            editorLabel.Dock = DockStyle.Top;
            editorLabel.Text = "المحرر:";
            editorLabel.Font = new Font("Tahoma", 9, FontStyle.Bold);
            editorLabel.TextAlign = ContentAlignment.MiddleRight;
            editorLabel.Height = 25;

            editor = new RichTextBox();
            editor.Dock = DockStyle.Fill;
            editor.Font = new Font("Consolas", 11);
            editor.Text = GetDefaultCode();
            editor.WordWrap = false;
            editor.ScrollBars = RichTextBoxScrollBars.Both;
            editor.TextChanged += Editor_TextChanged;
            editor.KeyDown += Editor_KeyDown;
            editor.SelectionChanged += Editor_SelectionChanged;

            editorPanel.Controls.Add(editor);
            editorPanel.Controls.Add(editorLabel);

            // لوحة الإخراج
            var outputPanel = new Panel();
            outputPanel.Dock = DockStyle.Fill;
            outputPanel.Padding = new Padding(5);

            var outputLabel = new Label();
            outputLabel.Dock = DockStyle.Top;
            outputLabel.Text = "الإخراج:";
            outputLabel.Font = new Font("Tahoma", 9, FontStyle.Bold);
            outputLabel.TextAlign = ContentAlignment.MiddleRight;
            outputLabel.Height = 25;

            outputBox = new RichTextBox();
            outputBox.Dock = DockStyle.Fill;
            outputBox.Font = new Font("Consolas", 9);
            outputBox.BackColor = Color.FromArgb(30, 30, 30);
            outputBox.ForeColor = Color.LightGreen;
            outputBox.ReadOnly = true;
            // Allow detecting file:// links so users can click to open logs
            outputBox.DetectUrls = true;
            outputBox.LinkClicked += OutputBox_LinkClicked;
            outputBox.WordWrap = false;
            outputBox.ScrollBars = RichTextBoxScrollBars.Both;

            outputPanel.Controls.Add(outputBox);
            outputPanel.Controls.Add(outputLabel);

            splitContainer.Panel1.Controls.Add(editorPanel);
            splitContainer.Panel2.Controls.Add(outputPanel);

            this.Controls.Add(splitContainer);
        }

        private void SetupStatusBar()
        {
            statusStrip = new StatusStrip();
            statusStrip.Dock = DockStyle.Bottom;

            statusLabel = new ToolStripStatusLabel();
            statusLabel.Text = "جاهز";
            statusLabel.Spring = true;

            lineLabel = new ToolStripStatusLabel();
            lineLabel.Text = "السطر: 1, العمود: 1";

            statusStrip.Items.Add(statusLabel);
            statusStrip.Items.Add(lineLabel);

            this.Controls.Add(statusStrip);
        }

        private ToolStripButton CreateToolStripButton(string text, string emoji, string tooltip)
        {
            var button = new ToolStripButton();
            button.Text = $"{emoji} {text}";
            button.ToolTipText = $"{text} ({tooltip})";
            button.DisplayStyle = ToolStripItemDisplayStyle.Text;
            button.AutoSize = true;
            return button;
        }

        private string GetDefaultCode()
        {
            return @"// برنامج مثال باللغة العربية
// مثال شامل يوضح الميزات الأساسية

برنامج مثال_شامل ؛

// تعريف الثوابت
ثابت ط = 3.14 ؛
ثابت الحد_الأقصى = 100 ؛

// تعريف المتغيرات
متغير س : عدد صحيح ؛
متغير ناتج : عدد صحيح ؛
متغير أرقام : مصفوفة[5] من عدد صحيح ؛

// تعيين القيم
س = 10 ؛
ناتج = س * 2 + 5 ؛
أرقام[0] = 10 ؛
أرقام[1] = 20 ؛

// جملة شرطية
إذا ناتج > 15 فإن
    اطبع ""الناتج كبير: "" ؛
    اطبع ناتج ؛
وإلا
    اطبع ""الناتج صغير: "" ؛
    اطبع ناتج ؛
نهاية ؛

// حلقة تكرار
لـ (ع = 1 إلى 5)
    اطبع ""العدد: "" ؛
    اطبع ع ؛
نهاية ؛

اطبع ""نهاية البرنامج"" ؛";
        }

        private void SetupShortcuts()
        {
            this.KeyPreview = true;
            this.KeyDown += MainForm_KeyDown;
        }

        private void SetupSyntaxHighlighting()
        {
            syntaxHighlightingTimer = new System.Windows.Forms.Timer(); // استخدام النوع الصريح
            syntaxHighlightingTimer.Interval = 500; // 0.5 second delay
            syntaxHighlightingTimer.Tick += (sender, e) => {
                syntaxHighlightingTimer.Stop();
                HighlightSyntax();
            };
        }

        private void HighlightSyntax()
        {
            if (isHighlighting || editor == null) return;

            isHighlighting = true;

            try
            {
                // Save state
                int selectionStart = editor.SelectionStart;
                int selectionLength = editor.SelectionLength;

                // RichTextBox doesn't expose BeginUpdate/EndUpdate. Use SuspendLayout/ResumeLayout
                // to reduce layout work during syntax highlighting.
                editor.SuspendLayout();

                // 1. Default color
                editor.SelectAll();
                editor.SelectionColor = identifierColor;

                // 2. Comments
                MatchCollection commentMatches = Regex.Matches(editor.Text, @"//[^\n]*");
                foreach (Match m in commentMatches)
                {
                    editor.Select(m.Index, m.Length);
                    editor.SelectionColor = commentColor;
                }

                // 3. Strings
                MatchCollection stringMatches = Regex.Matches(editor.Text, "\"[^\"\n]*\"");
                foreach (Match m in stringMatches)
                {
                    editor.Select(m.Index, m.Length);
                    editor.SelectionColor = stringColor;
                }

                // 4. Keywords
                string[] keywords = {
                    // البرنامج والكتل
                    "برنامج", "نهاية", "إجراء", "إرجاع",
                    // الشروط والحلقات
                    "إذا", "فإن", "وإلا", "بينما", "لـ", "إلى",
                    // الإدخال والإخراج
                    "اطبع", "اقرأ",
                    // المتغيرات والثوابت
                    "متغير", "ثابت",
                    // الأنواع
                    "عدد صحيح", "عدد عشري", "نص", "منطقي", "مصفوفة", "سجل", "حقل",
                    // المعاملات
                    "بالقيمة", "بالمرجع",
                    // العمليات المنطقية
                    "و", "أو", "ليس",
                    // القيم
                    "صح", "خطأ"
                };
                string keywordPattern = "\\b(" + string.Join("|", keywords) + ")\\b";
                MatchCollection keywordMatches = Regex.Matches(editor.Text, keywordPattern);
                foreach (Match m in keywordMatches)
                {
                    editor.Select(m.Index, m.Length);
                    editor.SelectionColor = keywordColor;
                }

                // 5. Numbers
                MatchCollection numberMatches = Regex.Matches(editor.Text, @"\b\d+(\.\d+)?\b");
                foreach (Match m in numberMatches)
                {
                    editor.Select(m.Index, m.Length);
                    editor.SelectionColor = numberColor;
                }

                // Restore state
                editor.Select(selectionStart, selectionLength);
                editor.SelectionColor = identifierColor; // Restore color at cursor
            }
            finally
            {
                editor.ResumeLayout();
                isHighlighting = false;
            }
        }

        private void UpdateTitle()
        {
            string filename = string.IsNullOrEmpty(currentFile) ? "غير محفوظ" : Path.GetFileName(currentFile);
            string modified = isModified ? " *" : "";
            this.Text = $"بيئة التطوير العربية - {filename}{modified}";
        }

        private void UpdateLineInfo()
        {
            if (editor != null && lineLabel != null)
            {
                int index = editor.SelectionStart;
                int line = editor.GetLineFromCharIndex(index) + 1;
                int firstChar = editor.GetFirstCharIndexFromLine(line - 1);
                int column = index - firstChar + 1;
                lineLabel.Text = $"السطر: {line}, العمود: {column}";
            }
        }

        private void Editor_SelectionChanged(object? sender, EventArgs e)
        {
            UpdateLineInfo();
        }

        private bool ConfirmSaveChanges()
        {
            if (!isModified) return true;

            var result = MessageBox.Show(
                "هل تريد حفظ التغييرات في الملف الحالي؟",
                "تأكيد حفظ",
                MessageBoxButtons.YesNoCancel,
                MessageBoxIcon.Question,
                MessageBoxDefaultButton.Button1,
                MessageBoxOptions.RightAlign | MessageBoxOptions.RtlReading
            );

            if (result == DialogResult.Yes)
            {
                return SaveFile();
            }
            else if (result == DialogResult.Cancel)
            {
                return false;
            }
            return true;
        }

        private void NewButton_Click(object? sender, EventArgs e)
        {
            if (!ConfirmSaveChanges()) return;

            currentFile = "";
            editor!.Text = GetDefaultCode();
            isModified = false;
            UpdateTitle();
            statusLabel!.Text = "تم إنشاء ملف جديد";
            UpdateLineInfo();
        }

        private void OpenButton_Click(object? sender, EventArgs e)
        {
            if (!ConfirmSaveChanges()) return;

            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = "ملفات العربية (*.arabic)|*.arabic|جميع الملفات (*.*)|*.*";
                dialog.Title = "فتح ملف برنامج عربي";
                dialog.CheckFileExists = true;

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    OpenFile(dialog.FileName);
                }
            }
        }

        private void OpenFile(string filename)
        {
            try
            {
                currentFile = filename;
                
                // محاولة قراءة الملف بترميزات مختلفة
                string content = TryReadFileWithEncoding(filename);
                editor!.Text = content;
                
                isModified = false;
                UpdateTitle();
                statusLabel!.Text = $"تم فتح الملف: {Path.GetFileName(currentFile)}";
                UpdateLineInfo();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ في فتح الملف: {ex.Message}", "خطأ",
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
                statusLabel!.Text = "فشل فتح الملف";
            }
        }

        private string TryReadFileWithEncoding(string filename)
        {
            // محاولة UTF-8 أولاً
            try
            {
                return File.ReadAllText(filename, Encoding.UTF8);
            }
            catch
            {
                // محاولة Windows-1256
                try
                {
                    return File.ReadAllText(filename, Encoding.GetEncoding(1256));
                }
                catch
                {
                    // استخدام الترميز الافتراضي
                    return File.ReadAllText(filename, Encoding.Default);
                }
            }
        }

        private bool SaveFile()
        {
            if (string.IsNullOrEmpty(currentFile))
            {
                return SaveAsFile();
            }

            try
            {
                // حفظ باستخدام UTF-8
                File.WriteAllText(currentFile, editor!.Text, Encoding.UTF8);
                isModified = false;
                UpdateTitle();
                statusLabel!.Text = $"تم حفظ الملف: {Path.GetFileName(currentFile)}";
                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ في حفظ الملف: {ex.Message}", "خطأ",
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
                statusLabel!.Text = "فشل حفظ الملف";
                return false;
            }
        }

        private bool SaveAsFile()
        {
            using (var dialog = new SaveFileDialog())
            {
                dialog.Filter = "ملفات العربية (*.arabic)|*.arabic|جميع الملفات (*.*)|*.*";
                dialog.Title = "حفظ ملف برنامج عربي";
                dialog.DefaultExt = ".arabic";
                dialog.FileName = string.IsNullOrEmpty(currentFile) ? "غير_مسمى.arabic" : Path.GetFileName(currentFile);

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    currentFile = dialog.FileName;
                    return SaveFile();
                }
                return false;
            }
        }

        private void SaveButton_Click(object? sender, EventArgs e)
        {
            SaveFile();
        }

        private void SaveAsButton_Click(object? sender, EventArgs e)
        {
            SaveAsFile();
        }

        private void RunButton_Click(object? sender, EventArgs e)
        {
            CompileAndRun(true);
        }

        private void CompileButton_Click(object? sender, EventArgs e)
        {
            CompileAndRun(false);
        }

        private void CompileAndRun(bool runAfterCompile)
        {
            if (!File.Exists(compilerPath))
            {
                MessageBox.Show(
                    $"المترجم '{compilerPath}' غير موجود في المجلد الحالي!\n\n" +
                    "تأكد من:\n" +
                    "1. بناء المترجم (C++) باستخدام CMake\n" +
                    "2. نسخ ArabicCompiler.exe إلى مجلد التطبيق\n" +
                    "3. أن المترجم في نفس مجلد ArabicIDE.exe",
                    "خطأ - المترجم غير موجود",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error
                );
                statusLabel!.Text = "المترجم غير موجود";
                return;
            }

            // التأكد من حفظ التغييرات قبل الترجمة
            if (isModified || string.IsNullOrEmpty(currentFile))
            {
                if (!SaveFile())
                {
                    statusLabel!.Text = "تم إلغاء الترجمة";
                    return;
                }
            }

            CompileProgram(editor!.Text, runAfterCompile);
        }

        private void LoadSettings()
        {
            try
            {
                if (File.Exists(settingsFile))
                {
                    string json = File.ReadAllText(settingsFile, Encoding.UTF8);
                    var doc = JsonDocument.Parse(json);
                    if (doc.RootElement.TryGetProperty("debugLexer", out var dj))
                    {
                        bool dbg = dj.GetBoolean();
                        if (debugLexerButton != null)
                            debugLexerButton.Checked = dbg;
                    }
                }
            }
            catch
            {
                // ignore errors
            }
        }

        private void SaveSettings()
        {
            try
            {
                var obj = new { debugLexer = debugLexerButton != null && debugLexerButton.Checked };
                var json = JsonSerializer.Serialize(obj, new JsonSerializerOptions { WriteIndented = true });
                File.WriteAllText(settingsFile, json, Encoding.UTF8);
            }
            catch
            {
                // ignore
            }
        }

        private void CompileProgram(string code, bool runAfterCompile)
        {
            statusLabel!.Text = "جاري الترجمة...";
            outputBox!.Clear();
            Application.DoEvents();

            string tempFile = Path.GetTempFileName();
            string baseName = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());

            try
            {
                // حفظ الملف المؤقت باستخدام UTF-8
                File.WriteAllText(tempFile, code, Encoding.UTF8);

                string outputOption = GetOutputOption();
                string selectedOutputType = GetSelectedOutputType();

                // تشغيل المترجم
                var result = RunCompiler(tempFile, outputOption);
                DisplayCompilationResult(result, baseName, selectedOutputType);

                if (result.ExitCode == 0)
                {
                    statusLabel.Text = "✅ الترجمة تمت بنجاح!";
                    if (runAfterCompile && selectedOutputType == "c")
                    {
                        TryRunCCode(baseName + ".c", baseName + ".exe");
                    }
                }
                else
                {
                    statusLabel.Text = "❌ فشل الترجمة!";
                }
            }
            catch (Exception ex)
            {
                outputBox.AppendText($"\n\nخطأ استثنائي أثناء الترجمة:\n{ex.Message}");
                statusLabel.Text = "❌ خطأ فادح في الترجمة!";
            }
            finally
            {
                CleanupTempFiles(tempFile, baseName);
            }
        }

        private string GetOutputOption()
        {
            return outputTypeComboBox!.SelectedIndex switch
            {
                0 => "--asm",
                1 => "--c", 
                2 => "--ir",
                3 => "--all",
                _ => "--asm"
            };
        }

        private string GetSelectedOutputType()
        {
            return outputTypeComboBox!.SelectedIndex switch
            {
                0 => "asm",
                1 => "c",
                2 => "ir",
                _ => "asm"
            };
        }

        private (int ExitCode, string Output, string Error) RunCompiler(string inputFile, string options)
        {
            using (var process = new Process())
            {
                string args = $"\"{inputFile}\" {options}";
                if (debugLexerButton != null && debugLexerButton.Checked)
                {
                    args += " --debug-lexer";
                }
                // show which compiler binary is used
                if (outputBox != null)
                {
                    outputBox.AppendText($"\n↪ استخدام المترجم: {compilerPath} {args}\n\n");
                }
                process.StartInfo = new ProcessStartInfo
                {
                    FileName = compilerPath,
                    Arguments = args,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true,
                    WorkingDirectory = Path.GetDirectoryName(Application.ExecutablePath) ?? Directory.GetCurrentDirectory(),
                    StandardOutputEncoding = Encoding.UTF8,
                    StandardErrorEncoding = Encoding.UTF8
                };

                process.Start();
                string output = process.StandardOutput.ReadToEnd();
                string error = process.StandardError.ReadToEnd();
                process.WaitForExit(15000); // 15 ثانية

                return (process.ExitCode, output, error);
            }
        }

        private void DisplayCompilationResult((int ExitCode, string Output, string Error) result, string baseName, string outputType)
        {
            var sb = new StringBuilder();
            sb.AppendLine("=== نتائج الترجمة ===");
            sb.AppendLine(result.Output.Trim());

            if (!string.IsNullOrEmpty(result.Error.Trim()))
            {
                sb.AppendLine("\n=== الأخطاء ===");
                sb.AppendLine(result.Error.Trim());
            }

            // عرض الملفات المولدة
            DisplayGeneratedFiles(sb, baseName, outputType);

            outputBox!.Text = sb.ToString();

            // تلوين الأخطاء
            if (!string.IsNullOrEmpty(result.Error.Trim()))
            {
                HighlightErrors();
            }
        }

        private void DisplayGeneratedFiles(StringBuilder sb, string baseName, string outputType)
        {
            string[] extensions = outputType switch
            {
                "asm" => new[] { ".asm" },
                "c" => new[] { ".c" },
                "ir" => new[] { "_intermediate.txt" },
                _ => new[] { ".asm", ".c", "_intermediate.txt" }
            };

            foreach (string ext in extensions)
            {
                string filePath = baseName + ext;
                if (File.Exists(filePath))
                {
                    try
                    {
                        string content = File.ReadAllText(filePath, Encoding.UTF8);
                        sb.AppendLine($"\n=== الكود المولد ({Path.GetFileName(filePath)}) ===");
                        sb.AppendLine(content);
                    }
                    catch (Exception ex)
                    {
                        sb.AppendLine($"\n=== خطأ في قراءة الملف المولد: {ex.Message} ===");
                    }
                }
            }
        }

        private void HighlightErrors()
        {
            int errorIndex = outputBox!.Text.IndexOf("=== الأخطاء ===");
            if (errorIndex >= 0)
            {
                outputBox.Select(errorIndex, outputBox.Text.Length - errorIndex);
                outputBox.SelectionColor = Color.Red;
                outputBox.Select(0, 0); // إلغاء التحديد
            }
        }

        private void TryRunCCode(string cFilePath, string exeFilePath)
        {
            if (!File.Exists(cFilePath))
            {
                outputBox!.AppendText($"\n\n⚠️ ملف C غير موجود للتنفيذ: {Path.GetFileName(cFilePath)}");
                return;
            }

            statusLabel!.Text = "جاري بناء كود C...";
            Application.DoEvents();

            if (CompileCCode(cFilePath, exeFilePath))
            {
                RunExecutable(exeFilePath);
            }
        }

        private bool CompileCCode(string cFile, string exeFile)
        {
            try
            {
                using (var process = new Process())
                {
                    process.StartInfo = new ProcessStartInfo
                    {
                        FileName = "gcc",
                        Arguments = $"\"{cFile}\" -o \"{exeFile}\"",
                        UseShellExecute = false,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        CreateNoWindow = true,
                        StandardOutputEncoding = Encoding.UTF8,
                        StandardErrorEncoding = Encoding.UTF8
                    };

                    process.Start();
                    string output = process.StandardOutput.ReadToEnd();
                    string error = process.StandardError.ReadToEnd();
                    process.WaitForExit(20000); // 20 seconds

                    if (!string.IsNullOrWhiteSpace(output))
                        outputBox!.AppendText($"\n\n=== مخرجات بناء GCC ===\n{output.Trim()}");
                    if (!string.IsNullOrWhiteSpace(error))
                        outputBox!.AppendText($"\n\n=== أخطاء بناء GCC ===\n{error.Trim()}");

                    return process.ExitCode == 0 && File.Exists(exeFile);
                }
            }
            catch (Win32Exception ex) when (ex.NativeErrorCode == 2)
            {
                MessageBox.Show(
                    "لم يتم العثور على مترجم 'gcc'.\n\n" +
                    "لتنفيذ البرامج، تحتاج إلى:\n" +
                    "1. تثبيت MinGW-w64 أو GCC\n" +
                    "2. إضافته إلى متغير البيئة PATH\n" +
                    "3. إعادة تشغيل البرنامج",
                    "خطأ - GCC غير موجود",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error
                );
                outputBox!.AppendText("\n\n❌ خطأ: لم يتم العثور على 'gcc'");
                return false;
            }
            catch (Exception ex)
            {
                outputBox!.AppendText($"\n\n❌ خطأ في بناء كود C: {ex.Message}");
                return false;
            }
        }

        private void RunExecutable(string exeFile)
        {
            if (!File.Exists(exeFile)) return;

            statusLabel!.Text = "جاري تنفيذ البرنامج في نافذة جديدة...";
            Application.DoEvents();

            try
            {
                using (var process = new Process())
                {
                    process.StartInfo = new ProcessStartInfo
                    {
                        FileName = exeFile,
                        UseShellExecute = false,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        CreateNoWindow = true,
                        StandardOutputEncoding = Encoding.UTF8,
                        StandardErrorEncoding = Encoding.UTF8
                    };

                    process.Start();
                    string output = process.StandardOutput.ReadToEnd();
                    string error = process.StandardError.ReadToEnd();
                    process.WaitForExit(10000); // 10 seconds

                    if (!string.IsNullOrWhiteSpace(output))
                        outputBox!.AppendText($"\n\n=== تنفيذ البرنامج ===\n{output}");
                    if (!string.IsNullOrWhiteSpace(error))
                        outputBox!.AppendText($"\n\n=== أخطاء تنفيذ البرنامج ===\n{error}");

                    statusLabel.Text = process.ExitCode == 0 ?
                        "✅ تم تنفيذ البرنامج بنجاح!" :
                        "⚠️ انتهى التنفيذ برمز خطأ";
                }
            }
            catch (Exception ex)
            {
                outputBox!.AppendText($"\n\n❌ خطأ في تشغيل البرنامج: {ex.Message}");
                statusLabel!.Text = "❌ فشل تشغيل البرنامج!";
            }
        }

        private void CleanupTempFiles(string tempFile, string baseName)
        {
            try
            {
                if (File.Exists(tempFile))
                    File.Delete(tempFile);

                string[] extensions = { ".c", ".exe", ".asm", "_intermediate.txt" };
                foreach (string ext in extensions)
                {
                    string file = baseName + ext;
                    if (File.Exists(file))
                        File.Delete(file);
                }
            }
            catch
            {
                // تجاهل أخطاء التنظيف
            }
        }

        private void Editor_TextChanged(object? sender, EventArgs e)
        {
            if (!isModified)
            {
                isModified = true;
                UpdateTitle();
            }

            if (syntaxHighlightingTimer != null)
            {
                syntaxHighlightingTimer.Stop();
                syntaxHighlightingTimer.Start();
            }
        }

        private void Editor_KeyDown(object? sender, KeyEventArgs e)
        {
            UpdateLineInfo();

            if (e.Control && e.KeyCode == Keys.S)
            {
                if (e.Shift)
                    SaveAsButton_Click(sender, e);
                else
                    SaveButton_Click(sender, e);
                e.Handled = true;
            }
            else if (e.Control && e.KeyCode == Keys.O)
            {
                OpenButton_Click(sender, e);
                e.Handled = true;
            }
            else if (e.Control && e.KeyCode == Keys.N)
            {
                NewButton_Click(sender, e);
                e.Handled = true;
            }
            else if (e.Control && e.KeyCode == Keys.B)
            {
                CompileButton_Click(sender, e);
                e.Handled = true;
            }
            else if (e.KeyCode == Keys.F5)
            {
                RunButton_Click(sender, e);
                e.Handled = true;
            }
        }

        private void MainForm_KeyDown(object? sender, KeyEventArgs e)
        {
            if (e.Control && e.KeyCode == Keys.Q)
            {
                this.Close();
            }
        }

        private void LoadExamples_Click(object? sender, EventArgs e)
        {
            try
            {
                string exeDir = Path.GetDirectoryName(Application.ExecutablePath) ?? Directory.GetCurrentDirectory();
                string repoRoot = Path.GetFullPath(Path.Combine(exeDir, ".."));
                string examplesDir = Path.Combine(repoRoot, "Examples");

                if (!Directory.Exists(examplesDir))
                {
                    MessageBox.Show($"مجلد الأمثلة غير موجود: {examplesDir}", "خطأ", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                var files = Directory.GetFiles(examplesDir, "*.arabic").OrderBy(f => f).ToList();
                if (files.Count == 0)
                {
                    MessageBox.Show("لا توجد أمثلة متاحة في مجلد Examples", "معلومة", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }

                // Create a simple selection dialog
                var form = new Form();
                form.Text = "اختر مثالاً";
                form.Size = new Size(400, 300);
                form.StartPosition = FormStartPosition.CenterParent;
                form.ShowInTaskbar = false;

                var listBox = new ListBox();
                listBox.Dock = DockStyle.Fill;
                listBox.Items.AddRange(files.Select(f => Path.GetFileName(f)).ToArray());
                listBox.DoubleClick += (s, e) => form.DialogResult = DialogResult.OK;

                var buttonPanel = new Panel();
                buttonPanel.Dock = DockStyle.Bottom;
                buttonPanel.Height = 40;
                buttonPanel.Padding = new Padding(5);

                var okButton = new Button();
                okButton.Text = "فتح";
                okButton.Dock = DockStyle.Right;
                okButton.Width = 80;
                okButton.Click += (s, e) => form.DialogResult = DialogResult.OK;

                var cancelButton = new Button();
                cancelButton.Text = "إلغاء";
                cancelButton.Dock = DockStyle.Right;
                cancelButton.Width = 80;
                cancelButton.Click += (s, e) => form.DialogResult = DialogResult.Cancel;

                buttonPanel.Controls.Add(okButton);
                buttonPanel.Controls.Add(cancelButton);

                form.Controls.Add(listBox);
                form.Controls.Add(buttonPanel);

                if (form.ShowDialog(this) == DialogResult.OK && listBox.SelectedIndex >= 0)
                {
                    string selectedFile = files[listBox.SelectedIndex];
                    OpenFile(selectedFile);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ في تحميل الأمثلة: {ex.Message}", "خطأ", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void RunAllExamples_Click(object? sender, EventArgs e)
        {
            // Fire-and-forget the async runner but keep UI responsive
            _ = RunAllExamplesAsync();
        }

        private async Task RunAllExamplesAsync()
        {
            if (runAllButton != null)
                runAllButton.Enabled = false;

            try
            {
                statusLabel!.Text = "جاري تشغيل جميع الأمثلة...";
                outputBox!.Clear();

                string exeDir = Path.GetDirectoryName(Application.ExecutablePath) ?? Directory.GetCurrentDirectory();
                string repoRoot = Path.GetFullPath(Path.Combine(exeDir, ".."));
                // run end-to-end script (compile .arabic -> build C -> run)
                string script = Path.Combine(repoRoot, "scripts", "run_end_to_end.ps1");
                string logsDir = Path.Combine(repoRoot, "build_tests_logs");

                if (!File.Exists(script))
                {
                    // If the PowerShell script is missing, fall back to an internal managed runner
                    outputBox.AppendText($"⚠️ لم يتم العثور على السكربت: {script}. سيتم استخدام المُنفّذ الداخلي بدلاً منه.\n");
                    await RunEndToEndInternalAsync(repoRoot, logsDir);
                    statusLabel.Text = "تم تشغيل الأمثلة (المنفذ الداخلي) - انظر الإخراج";
                    return;
                }

                var psi = new ProcessStartInfo
                {
                    FileName = "powershell.exe",
                    Arguments = $"-NoProfile -ExecutionPolicy Bypass -File \"{script}\"",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true
                };

                using (var proc = Process.Start(psi))
                {
                    string sout = await proc.StandardOutput.ReadToEndAsync();
                    string serr = await proc.StandardError.ReadToEndAsync();
                    proc.WaitForExit();

                    if (!string.IsNullOrWhiteSpace(sout))
                        outputBox.AppendText("=== سكربت: المخرجات ===\n" + sout + "\n");
                    if (!string.IsNullOrWhiteSpace(serr))
                        outputBox.AppendText("=== سكربت: الأخطاء ===\n" + serr + "\n");

                    string summaryFile = Path.Combine(logsDir, "summary.txt");
                    if (File.Exists(summaryFile))
                    {
                        outputBox.AppendText("\n=== Summary ===\n");

                        // Read lines and append a clickable file:// link per summary entry
                        var lines = File.ReadAllLines(summaryFile, Encoding.UTF8);
                        foreach (var l in lines)
                        {
                            if (string.IsNullOrWhiteSpace(l)) continue;
                            // expect format: name: status
                            var parts = l.Split(':', 2);
                            string name = parts[0].Trim();
                            string status = parts.Length > 1 ? parts[1].Trim() : "";
                            string logFile = Path.Combine(logsDir, name + ".log");

                            if (File.Exists(logFile))
                            {
                                // Use file:// URI so RichTextBox auto-detects it as a link
                                var uri = new Uri(logFile).AbsoluteUri;
                                outputBox.AppendText($"{name}: {status} ");
                                outputBox.AppendText(uri + "\n");
                            }
                            else
                            {
                                outputBox.AppendText($"{name}: {status} (log missing)\n");
                            }
                        }
                    }
                    else
                    {
                        outputBox.AppendText($"\nلم يتم العثور على ملف الملخص: {summaryFile}\n");
                    }

                    statusLabel.Text = "تم تشغيل الأمثلة - انظر الإخراج";
                }
            }
            catch (Exception ex)
            {
                outputBox!.AppendText($"\n\n❌ خطأ أثناء تشغيل الأمثلة: {ex.Message}");
                statusLabel!.Text = "خطأ في تشغيل الأمثلة";
            }
            finally
            {
                if (runAllButton != null)
                    runAllButton.Enabled = true;
            }
        }

        private void OutputBox_LinkClicked(object sender, LinkClickedEventArgs e)
        {
            try
            {
                string link = e.LinkText;
                // Support file:// URIs
                if (link.StartsWith("file://", StringComparison.OrdinalIgnoreCase))
                {
                    var uri = new Uri(link);
                    string path = uri.LocalPath;
                    if (File.Exists(path))
                    {
                        ShowLogViewer(Path.GetFileName(path), File.ReadAllText(path, Encoding.UTF8));
                        return;
                    }
                }

                // Fallback: if it's a local path
                if (File.Exists(link))
                {
                    ShowLogViewer(Path.GetFileName(link), File.ReadAllText(link, Encoding.UTF8));
                    return;
                }

                MessageBox.Show($"لا يمكن فتح الرابط: {link}", "خطأ", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ عند فتح السجل: {ex.Message}", "خطأ", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void ShowLogViewer(string title, string content)
        {
            var dlg = new Form();
            dlg.Text = "سجل: " + title;
            dlg.Size = new Size(900, 600);
            dlg.StartPosition = FormStartPosition.CenterParent;

            var rtb = new RichTextBox();
            rtb.Dock = DockStyle.Fill;
            rtb.Font = new Font("Consolas", 10);
            rtb.ReadOnly = true;
            rtb.BackColor = Color.FromArgb(20, 20, 20);
            rtb.ForeColor = Color.LightGreen;
            rtb.WordWrap = false;
            rtb.Text = content;

            dlg.Controls.Add(rtb);
            dlg.ShowDialog(this);
        }

        private async Task RunEndToEndInternalAsync(string repoRoot, string logsDir)
        {
            try
            {
                Directory.CreateDirectory(logsDir);

                // Locate compiler
                string[] candidates = new[] {
                    Path.Combine(repoRoot, "Compiler", "ArabicCompiler.exe"),
                    Path.Combine(repoRoot, "Compiler", "build", "Release", "ArabicCompiler.exe"),
                    Path.Combine(repoRoot, "Compiler", "build", "ArabicCompiler.exe"),
                    Path.Combine(repoRoot, "ArabicCompiler.exe")
                };
                string compiler = candidates.FirstOrDefault(p => File.Exists(p));
                if (compiler == null)
                {
                    outputBox!.AppendText("❌ لم يتم العثور على ArabicCompiler.exe في المسارات المتوقعة. تأكد من بناء المترجم أولاً.\n");
                    return;
                }

                var summary = new List<string>();
                string examplesDir = Path.Combine(repoRoot, "Examples");
                var files = Directory.GetFiles(examplesDir, "*.arabic");
                foreach (var src in files)
                {
                    string baseName = Path.GetFileNameWithoutExtension(src);
                    string logFile = Path.Combine(logsDir, baseName + ".log");
                    var lines = new List<string>();
                    lines.Add($"Source: {src}");
                    lines.Add($"Started: {DateTime.UtcNow:o}");

                    try
                    {
                        outputBox.AppendText($"↳ تشغيل المترجم على: {baseName}...\n");
                        // Run ArabicCompiler
                        var psi = new ProcessStartInfo
                        {
                            FileName = compiler,
                            Arguments = $"\"{src}\" --c --asm --ir",
                            UseShellExecute = false,
                            RedirectStandardOutput = true,
                            RedirectStandardError = true,
                            CreateNoWindow = true,
                            StandardOutputEncoding = Encoding.UTF8,
                            StandardErrorEncoding = Encoding.UTF8
                        };
                        using (var p = Process.Start(psi))
                        {
                            string sout = await p.StandardOutput.ReadToEndAsync();
                            string serr = await p.StandardError.ReadToEndAsync();
                            p.WaitForExit();
                            lines.Add($"ArabicCompiler exit code: {p.ExitCode}");
                            lines.Add("ArabicCompiler stdout:");
                            lines.AddRange(sout.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));
                            lines.Add("ArabicCompiler stderr:");
                            lines.AddRange(serr.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));

                            if (p.ExitCode != 0)
                            {
                                lines.Add("Result: compile-source-failed");
                                summary.Add($"{baseName}: compile-source-failed");
                                File.WriteAllLines(logFile, lines, Encoding.UTF8);
                                continue;
                            }
                        }

                        string baseOut = Path.Combine(Path.GetDirectoryName(src) ?? examplesDir, baseName + "_output");
                        string cfile = baseOut + ".c";
                        if (!File.Exists(cfile))
                        {
                            lines.Add("Result: c-not-generated");
                            summary.Add($"{baseName}: c-not-generated");
                            File.WriteAllLines(logFile, lines, Encoding.UTF8);
                            continue;
                        }

                        // Compile with gcc if available
                        string exeTarget = Path.Combine(logsDir, baseName + ".exe");
                        var gccPsi = new ProcessStartInfo
                        {
                            FileName = "gcc",
                            Arguments = $"\"{cfile}\" -o \"{exeTarget}\"",
                            UseShellExecute = false,
                            RedirectStandardOutput = true,
                            RedirectStandardError = true,
                            CreateNoWindow = true
                        };
                        try
                        {
                            using (var gp = Process.Start(gccPsi))
                            {
                                string gsout = await gp.StandardOutput.ReadToEndAsync();
                                string gserr = await gp.StandardError.ReadToEndAsync();
                                gp.WaitForExit();
                                if (gp.ExitCode != 0 || !File.Exists(exeTarget))
                                {
                                    lines.Add($"gcc failed with exit {gp.ExitCode}");
                                    lines.AddRange(gserr.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));
                                    summary.Add($"{baseName}: gcc-failed");
                                    File.WriteAllLines(logFile, lines, Encoding.UTF8);
                                    continue;
                                }
                            }
                        }
                        catch (Win32Exception)
                        {
                            lines.Add("gcc not found in PATH: skipping native build/run");
                            summary.Add($"{baseName}: skipped-no-gcc");
                            File.WriteAllLines(logFile, lines, Encoding.UTF8);
                            continue;
                        }

                        // Run the produced exe
                        try
                        {
                            using (var rp = Process.Start(new ProcessStartInfo { FileName = exeTarget, RedirectStandardOutput = true, RedirectStandardError = true, UseShellExecute = false, CreateNoWindow = true }))
                            {
                                string rout = await rp.StandardOutput.ReadToEndAsync();
                                string rerr = await rp.StandardError.ReadToEndAsync();
                                rp.WaitForExit();
                                lines.Add($"Program exit code: {rp.ExitCode}");
                                lines.Add("Program stdout:");
                                lines.AddRange(rout.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));
                                lines.Add("Program stderr:");
                                lines.AddRange(rerr.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));
                                if (rp.ExitCode == 0)
                                {
                                    lines.Add("Result: ok");
                                    summary.Add($"{baseName}: ok");
                                }
                                else
                                {
                                    lines.Add("Result: runtime-failed");
                                    summary.Add($"{baseName}: runtime-failed");
                                }
                            }
                        }
                        catch (Exception ex)
                        {
                            lines.Add($"Exception running exe: {ex.Message}");
                            summary.Add($"{baseName}: runtime-exception");
                        }

                        File.WriteAllLines(logFile, lines, Encoding.UTF8);
                    }
                    catch (Exception ex)
                    {
                        lines.Add($"Exception: {ex}");
                        File.WriteAllLines(logFile, lines, Encoding.UTF8);
                        summary.Add($"{baseName}: exception");
                    }
                }

                // Write summary
                string summaryFile = Path.Combine(logsDir, "summary.txt");
                File.WriteAllLines(summaryFile, summary, Encoding.UTF8);

                // Show summary in outputBox
                outputBox!.AppendText("\n=== Summary ===\n");
                foreach (var s in summary)
                {
                    outputBox.AppendText(s + "\n");
                }
            }
            catch (Exception ex)
            {
                outputBox!.AppendText($"\n❌ خطأ في المنفّذ الداخلي: {ex.Message}\n");
            }
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (!ConfirmSaveChanges())
            {
                e.Cancel = true;
            }
            base.OnFormClosing(e);
        }
    }
}

