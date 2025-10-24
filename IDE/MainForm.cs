using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Text;

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
        private ToolStripStatusLabel? statusLabel;
        private ToolStripStatusLabel? lineLabel;
        private StatusStrip? statusStrip;
        
        private string compilerPath = "ArabicCompiler.exe";
        private string currentFile = "";
        private bool isModified = false;
        
        public MainForm()
        {
            InitializeComponent();
            SetupComponents();
            UpdateTitle();
            SetupSyntaxHighlighting();
        }
        
        private void InitializeComponent()
        {
            // هذا لمنع التعارض مع التصميم اليدوي
        }
        
        private void SetupComponents()
        {
            this.Text = "بيئة التطوير العربية - Arabic IDE";
            this.Size = new Size(1200, 800);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.Font = new Font("Tahoma", 9);
            
            // شريط الأدوات الرئيسي
            var mainToolStrip = new ToolStrip();
            mainToolStrip.Dock = DockStyle.Top;
            mainToolStrip.GripStyle = ToolStripGripStyle.Hidden;
            
            // أزرار الملفات
            newButton = CreateToolStripButton("جديد", "NewDocument.png", "Ctrl+N");
            openButton = CreateToolStripButton("فتح", "OpenFolder.png", "Ctrl+O");
            saveButton = CreateToolStripButton("حفظ", "Save.png", "Ctrl+S");
            saveAsButton = CreateToolStripButton("حفظ كـ", "SaveAs.png", "Ctrl+Shift+S");
            
            newButton!.Click += NewButton_Click;
            openButton!.Click += OpenButton_Click;
            saveButton!.Click += SaveButton_Click;
            saveAsButton!.Click += SaveAsButton_Click;
            
            mainToolStrip.Items.Add(newButton);
            mainToolStrip.Items.Add(openButton);
            mainToolStrip.Items.Add(saveButton);
            mainToolStrip.Items.Add(saveAsButton);
            mainToolStrip.Items.Add(new ToolStripSeparator());
            
            // أزرار الترجمة
            runButton = CreateToolStripButton("ترجمة وتنفيذ", "Run.png", "F5");
            var compileButton = CreateToolStripButton("ترجمة فقط", "Build.png", "Ctrl+B");
            
            runButton!.Click += RunButton_Click;
            compileButton!.Click += CompileButton_Click;
            
            mainToolStrip.Items.Add(runButton);
            mainToolStrip.Items.Add(compileButton);
            mainToolStrip.Items.Add(new ToolStripSeparator());
            
            // نوع الإخراج
            mainToolStrip.Items.Add(new ToolStripLabel("نوع الإخراج:"));
            outputTypeComboBox = new ComboBox();
            outputTypeComboBox.Items.AddRange(new string[] { "كود Assembly", "كود C", "كود وسيط" });
            outputTypeComboBox.SelectedIndex = 0;
            outputTypeComboBox.Width = 120;
            
            var comboHost = new ToolStripControlHost(outputTypeComboBox);
            mainToolStrip.Items.Add(comboHost);
            
            // محرر النص مع تقسيم
            var splitContainer = new SplitContainer();
            splitContainer.Dock = DockStyle.Fill;
            splitContainer.Orientation = Orientation.Vertical;
            splitContainer.SplitterDistance = 600;
            
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
            editor.Text = @"// برنامج مثال باللغة العربية
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
            editor.WordWrap = false;
            editor.SelectionAlignment = HorizontalAlignment.Right;
            editor.TextChanged += Editor_TextChanged;
            editor.KeyDown += Editor_KeyDown;
            
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
            
            outputPanel.Controls.Add(outputBox);
            outputPanel.Controls.Add(outputLabel);
            
            splitContainer.Panel1.Controls.Add(editorPanel);
            splitContainer.Panel2.Controls.Add(outputPanel);
            
            // شريط الحالة
            statusStrip = new StatusStrip();
            statusStrip.Dock = DockStyle.Bottom;
            
            statusLabel = new ToolStripStatusLabel();
            statusLabel.Text = "جاهز";
            statusLabel.Spring = true;
            
            lineLabel = new ToolStripStatusLabel();
            lineLabel.Text = "السطر: 1, العمود: 1";
            
            statusStrip.Items.Add(statusLabel);
            statusStrip.Items.Add(lineLabel);
            
            // إضافة العناصر للنموذج
            this.Controls.Add(splitContainer);
            this.Controls.Add(mainToolStrip);
            this.Controls.Add(statusStrip);
            
            // إعداد اختصارات لوحة المفاتيح
            SetupShortcuts();
        }
        
        private ToolStripButton CreateToolStripButton(string text, string imageName, string tooltip)
        {
            var button = new ToolStripButton(text);
            button.ToolTipText = tooltip;
            return button;
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
            if (editor != null)
            {
                int line = editor.GetLineFromCharIndex(editor.SelectionStart) + 1;
                int column = editor.SelectionStart - editor.GetFirstCharIndexFromLine(line - 1) + 1;
                if (lineLabel != null)
                {
                    lineLabel.Text = $"السطر: {line}, العمود: {column}";
                }
            }
        }
        
        private void NewButton_Click(object? sender, EventArgs e)
        {
            if (isModified)
            {
                var result = MessageBox.Show("هل تريد حفظ التغييرات في الملف الحالي؟", "تأكيد", 
                                           MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
                
                if (result == DialogResult.Yes)
                {
                    SaveButton_Click(sender, e);
                }
                else if (result == DialogResult.Cancel)
                {
                    return;
                }
            }
            
            currentFile = "";
            if (editor != null)
            {
                editor.Text = "برنامج برنامج_جديد؛\n\n// اكتب كودك هنا\nاطبع \"مرحباً بالعالم!\";\n\nنهاية؛";
            }
            isModified = false;
            UpdateTitle();
        }
        
        private void OpenButton_Click(object? sender, EventArgs e)
        {
            if (isModified)
            {
                var result = MessageBox.Show("هل تريد حفظ التغييرات في الملف الحالي؟", "تأكيد", 
                                           MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
                
                if (result == DialogResult.Yes)
                {
                    SaveButton_Click(sender, e);
                }
                else if (result == DialogResult.Cancel)
                {
                    return;
                }
            }
            
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = "ملفات العربية (*.arabic)|*.arabic|جميع الملفات (*.*)|*.*";
                dialog.Title = "فتح ملف برنامج عربي";
                
                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        currentFile = dialog.FileName;
                        if (editor != null)
                        {
                            editor.Text = File.ReadAllText(currentFile, Encoding.UTF8);
                        }
                        isModified = false;
                        UpdateTitle();
                        if (statusLabel != null)
                        {
                            statusLabel.Text = "تم فتح الملف: " + Path.GetFileName(currentFile);
                        }
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show($"خطأ في فتح الملف: {ex.Message}", "خطأ", 
                                      MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }
        
        private void SaveButton_Click(object? sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(currentFile))
            {
                SaveAsButton_Click(sender, e);
                return;
            }
            
            try
            {
                if (editor != null)
                {
                    File.WriteAllText(currentFile, editor.Text, Encoding.UTF8);
                }
                isModified = false;
                UpdateTitle();
                if (statusLabel != null)
                {
                    statusLabel.Text = "تم حفظ الملف: " + Path.GetFileName(currentFile);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ في حفظ الملف: {ex.Message}", "خطأ", 
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        
        private void SaveAsButton_Click(object? sender, EventArgs e)
        {
            using (var dialog = new SaveFileDialog())
            {
                dialog.Filter = "ملفات العربية (*.arabic)|*.arabic|جميع الملفات (*.*)|*.*";
                dialog.Title = "حفظ ملف برنامج عربي";
                dialog.DefaultExt = ".arabic";
                
                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    currentFile = dialog.FileName;
                    SaveButton_Click(sender, e);
                }
            }
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
                MessageBox.Show($"المترجم {compilerPath} غير موجود!", "خطأ", 
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            
            if (editor != null)
            {
                SaveButton_Click(this, EventArgs.Empty); // حفظ تلقائي
                CompileProgram(editor.Text, runAfterCompile);
            }
        }
        
        private void CompileProgram(string code, bool runAfterCompile)
        {
            try
            {
                if (statusLabel != null)
                {
                    statusLabel.Text = "جاري الترجمة...";
                }
                if (outputBox != null)
                {
                    outputBox.Clear();
                }
                Application.DoEvents();
                
                string tempFile = Path.GetTempFileName() + ".arabic";
                File.WriteAllText(tempFile, code, Encoding.UTF8);
                
                string outputType = "asm";
                if (outputTypeComboBox != null)
                {
                    outputType = outputTypeComboBox.SelectedIndex switch
                    {
                        0 => "asm",
                        1 => "c", 
                        2 => "ir",
                        _ => "asm"
                    };
                }
                
                // تنفيذ المترجم
                Process process = new Process();
                process.StartInfo.FileName = compilerPath;
                process.StartInfo.Arguments = $"\"{tempFile}\" {outputType}";
                process.StartInfo.UseShellExecute = false;
                process.StartInfo.RedirectStandardOutput = true;
                process.StartInfo.RedirectStandardError = true;
                process.StartInfo.CreateNoWindow = true;
                process.StartInfo.WorkingDirectory = Directory.GetCurrentDirectory();
                process.StartInfo.StandardOutputEncoding = Encoding.UTF8;
                process.StartInfo.StandardErrorEncoding = Encoding.UTF8;
                
                process.Start();
                
                string output = process.StandardOutput.ReadToEnd();
                string error = process.StandardError.ReadToEnd();
                
                process.WaitForExit(10000); // انتظار 10 ثواني كحد أقصى
                
                // عرض النتائج
                StringBuilder result = new StringBuilder();
                result.AppendLine("=== نتائج الترجمة ===");
                result.AppendLine(output);
                
                if (!string.IsNullOrEmpty(error))
                {
                    result.AppendLine("\n=== الأخطاء ===");
                    result.AppendLine(error);
                }
                
                // عرض الكود المولد
                string outputFile = $"output.{outputType}";
                if (File.Exists(outputFile))
                {
                    string generatedCode = File.ReadAllText(outputFile, Encoding.UTF8);
                    result.AppendLine($"\n=== الكود المولد ({outputFile}) ===");
                    result.AppendLine(generatedCode);
                }
                
                if (outputBox != null)
                {
                    outputBox.Text = result.ToString();
                
                    // تلوين الأخطاء
                    if (!string.IsNullOrEmpty(error))
                    {
                        int errorIndex = outputBox.Text.IndexOf("=== الأخطاء ===");
                        if (errorIndex >= 0)
                        {
                            outputBox.Select(errorIndex, outputBox.Text.Length - errorIndex);
                            outputBox.SelectionColor = Color.Red;
                            outputBox.Select(0, 0); // إلغاء التحديد
                        }
                    }
                }
                
                if (process.ExitCode == 0)
                {
                    if (statusLabel != null)
                    {
                        statusLabel.Text = "✅ الترجمة تمت بنجاح!";
                    }
                    
                    if (runAfterCompile && outputType == "c")
                    {
                        // محاولة تنفيذ كود C
                        TryRunCCode();
                    }
                }
                else
                {
                    if (statusLabel != null)
                    {
                        statusLabel.Text = "❌ فشل الترجمة!";
                    }
                }
                
                File.Delete(tempFile);
                
            }
            catch (Exception ex)
            {
                if (outputBox != null)
                {
                    outputBox.Text = $"خطأ في الترجمة: {ex.Message}";
                }
                if (statusLabel != null)
                {
                    statusLabel.Text = "❌ خطأ في الترجمة!";
                }
            }
        }
        
        private void TryRunCCode()
        {
            try
            {
                if (File.Exists("output.c"))
                {
                    if (statusLabel != null)
                    {
                        statusLabel.Text = "جاري تنفيذ البرنامج...";
                    }
                    Application.DoEvents();
                    
                    // محاولة ترجمة وتنفيذ كود C
                    Process compileProcess = new Process();
                    compileProcess.StartInfo.FileName = "gcc";
                    compileProcess.StartInfo.Arguments = "output.c -o output.exe";
                    compileProcess.StartInfo.UseShellExecute = false;
                    compileProcess.StartInfo.RedirectStandardOutput = true;
                    compileProcess.StartInfo.RedirectStandardError = true;
                    compileProcess.StartInfo.CreateNoWindow = true;
                    
                    compileProcess.Start();
                    string compileOutput = compileProcess.StandardOutput.ReadToEnd();
                    string compileError = compileProcess.StandardError.ReadToEnd();
                    compileProcess.WaitForExit();
                    
                    if (compileProcess.ExitCode == 0 && File.Exists("output.exe"))
                    {
                        // تنفيذ البرنامج المترجم
                        Process runProcess = new Process();
                        runProcess.StartInfo.FileName = "output.exe";
                        runProcess.StartInfo.UseShellExecute = false;
                        runProcess.StartInfo.RedirectStandardOutput = true;
                        runProcess.StartInfo.RedirectStandardError = true;
                        runProcess.StartInfo.CreateNoWindow = true;
                        
                        runProcess.Start();
                        string runOutput = runProcess.StandardOutput.ReadToEnd();
                        string runError = runProcess.StandardError.ReadToEnd();
                        runProcess.WaitForExit();
                        
                        if (outputBox != null)
                        {
                            outputBox.AppendText($"\n\n=== تنفيذ البرنامج ===\n{runOutput}");
                            if (!string.IsNullOrEmpty(runError))
                            {
                                outputBox.AppendText($"\nأخطاء التنفيذ:\n{runError}");
                            }
                        }
                        
                        if (statusLabel != null)
                        {
                            statusLabel.Text = "✅ تم تنفيذ البرنامج بنجاح!";
                        }
                    }
                    else
                    {
                        if (outputBox != null)
                        {
                            outputBox.AppendText($"\n\nخطأ في ترجمة كود C:\n{compileError}");
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                if (outputBox != null)
                {
                    outputBox.AppendText($"\n\nخطأ في التنفيذ: {ex.Message}");
                }
            }
        }
        
        private void Editor_TextChanged(object? sender, EventArgs e)
        {
            isModified = true;
            UpdateTitle();
            UpdateLineInfo();
        }
        
        private void Editor_KeyDown(object? sender, KeyEventArgs e)
        {
            UpdateLineInfo();
            
            // اختصارات لوحة المفاتيح
            if (e.Control && e.KeyCode == Keys.S)
            {
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
            // اختصارات عامة
        }
        
        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (isModified)
            {
                var result = MessageBox.Show("هل تريد حفظ التغييرات قبل الخروج؟", "تأكيد", 
                                           MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
                
                if (result == DialogResult.Yes)
                {
                    SaveButton_Click(this, EventArgs.Empty);
                }
                else if (result == DialogResult.Cancel)
                {
                    e.Cancel = true;
                }
            }
            
            base.OnFormClosing(e);
        }
    }
}