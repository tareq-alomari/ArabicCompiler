using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Text;
using System.ComponentModel;

namespace ArabicIDE
{
    public partial class MainForm : Form
    {
        private RichTextBox? editor;
        private RichTextBox? outputBox;
        private ComboBox? outputTypeComboBox;
        private ToolStripButton? runButton;
        private ToolStripButton? openButton;
        private ToolStripButton? saveButton;
        private ToolStripButton? saveAsButton;
        private ToolStripButton? newButton;
        private ToolStripButton? compileButton;
        private ToolStripStatusLabel? statusLabel;
        private ToolStripStatusLabel? lineLabel;
        private StatusStrip? statusStrip;

        private string compilerPath = "ArabicCompiler.exe";
        private string currentFile = "";
        private bool isModified = false;

        public MainForm()
        {
            // تسجيل ترميز Windows-1256
            Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
            
            InitializeComponent();
            SetupComponents();
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

            runButton!.Click += RunButton_Click;
            compileButton!.Click += CompileButton_Click;

            mainToolStrip.Items.Add(runButton);
            mainToolStrip.Items.Add(compileButton);
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
برنامج مثال؛

متغير س = 10؛
متغير ناتج = 0؛

ناتج = س * 2 + 5؛

اذا ناتج > 15 فان
    اطبع ""الناتج كبير: ""؛
    اطبع ناتج؛
وإلا
    اطبع ""الناتج صغير: ""؛
    اطبع ناتج؛
نهاية؛

اطبع ""نهاية البرنامج""؛";
        }

        private void SetupShortcuts()
        {
            this.KeyPreview = true;
            this.KeyDown += MainForm_KeyDown;
        }

        private void SetupSyntaxHighlighting()
        {
            // يمكن إضافة تلوين النصوص هنا لاحقاً
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
            statusLabel!.Text = "جاهز";
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
                process.StartInfo = new ProcessStartInfo
                {
                    FileName = compilerPath,
                    Arguments = $"\"{inputFile}\" {options}",
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
                    process.WaitForExit(20000); // 20 ثانية

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

            statusLabel!.Text = "جاري تنفيذ البرنامج...";
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
                    process.WaitForExit(10000); // 10 ثواني

                    outputBox!.AppendText($"\n\n=== تنفيذ البرنامج ===\n{output.Trim()}");

                    if (!string.IsNullOrEmpty(error.Trim()))
                    {
                        outputBox.AppendText($"\n=== أخطاء التنفيذ ===\n{error.Trim()}");
                    }

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