using System;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace ArabicIDE
{
    internal static class Program
    {
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            
            // التحقق من وجود المترجم
            if (!File.Exists("ArabicCompiler.exe"))
            {
                MessageBox.Show("المترجم ArabicCompiler.exe غير موجود في مجلد التطبيق.\n" +
                              "يرجى بناء المترجم أولاً ثم تشغيل الـ IDE مرة أخرى.\n\n" +
                              "يمكنك بناء المترجم باستخدام CMake أو Visual Studio.", 
                              "خطأ", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            
            // التحقق من وجود GCC إذا كان مستخدم
            try
            {
                Process process = new Process();
                process.StartInfo.FileName = "gcc";
                process.StartInfo.Arguments = "--version";
                process.StartInfo.UseShellExecute = false;
                process.StartInfo.CreateNoWindow = true;
                process.Start();
                process.WaitForExit();
            }
            catch
            {
                MessageBox.Show("ملاحظة: لم يتم العثور على GCC.\n" +
                              "لتنفيذ البرامج، تحتاج إلى تثبيت GCC أو MinGW.", 
                              "ملاحظة", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            
            try
            {
                Application.Run(new MainForm());
            }
            catch (Exception ex)
            {
                MessageBox.Show($"حدث خطأ غير متوقع: {ex.Message}", "خطأ", 
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}