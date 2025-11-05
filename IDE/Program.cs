using System;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Text;

namespace ArabicIDE
{
    internal static class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                // CLI helper: run end-to-end tests headless when requested
                if (args != null && args.Length > 0 && args[0] == "--run-tests")
                {
                    string exeDir = AppDomain.CurrentDomain.BaseDirectory;
                    // find repository root by walking up until we find a 'scripts' folder
                    string dir = exeDir;
                    string repoRoot = null;
                    for (int i = 0; i < 6; i++)
                    {
                        string candidate = Path.Combine(dir, "scripts");
                        if (Directory.Exists(candidate)) { repoRoot = Directory.GetParent(candidate).FullName; break; }
                        var parent = Directory.GetParent(dir);
                        if (parent == null) break;
                        dir = parent.FullName;
                    }
                    if (repoRoot == null) repoRoot = Path.GetFullPath(Path.Combine(exeDir, "..", "..", ".."));

                    string script = Path.Combine(repoRoot, "scripts", "run_end_to_end.ps1");
                    if (!File.Exists(script))
                    {
                        Console.WriteLine($"run-tests: script not found: {script}");
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
                    using (var p = Process.Start(psi))
                    {
                        string outp = p.StandardOutput.ReadToEnd();
                        string errp = p.StandardError.ReadToEnd();
                        p.WaitForExit();
                        Console.WriteLine(outp);
                        if (!string.IsNullOrWhiteSpace(errp)) Console.Error.WriteLine(errp);
                    }
                    return;
                }
              
              
                
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                
                // التحقق من المتطلبات الأساسية
                if (!CheckBasicRequirements())
                {
                    return; // إغلاق التطبيق إذا قرر المستخدم ذلك
                }
                
                // تشغيل التطبيق الرئيسي
                Application.Run(new MainForm());
            }
            catch (Exception ex)
            {
                MessageBox.Show($"خطأ فادح أثناء بدء التشغيل: {ex.Message}\n\n" +
                                $"نوع الخطأ: {ex.GetType().Name}",
                                "خطأ - بيئة التطوير العربية",
                                MessageBoxButtons.OK, 
                                MessageBoxIcon.Error);
            }
        }

        // تم تعديلها لترجع قيمة bool
        static bool CheckBasicRequirements()
        {
            //string compilerName = "ArabicCompiler.exe";
            string compilerName = "my_compiler.exe";
            string currentDirectory = Directory.GetCurrentDirectory();
            string compilerPath = Path.Combine(currentDirectory, compilerName);

            if (!File.Exists(compilerPath))
            {
                var result = MessageBox.Show(
                    $"المترجم '{compilerName}' غير موجود في المجلد الحالي.\n\n" +
                    $"المسار الذي تم البحث فيه:\n{currentDirectory}\n\n" +
                    "بدون المترجم، يمكنك استخدام المحرر للقراءة والكتابة فقط، ولكن لن تعمل الترجمة أو التنفيذ.\n\n" +
                    "هل تريد الاستمرار في وضع المحرر فقط؟",
                    "تحذير - المترجم غير موجود",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Warning);

                return (result == DialogResult.Yes); // إرجاع true إذا وافق المستخدم، و false إذا ضغط 'لا'
            }
            
            return true; // المترجم موجود
        }
    }
}