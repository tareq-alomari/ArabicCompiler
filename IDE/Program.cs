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
        static void Main()
        {
            try
            {
                // إعداد ترميز UTF-8 للمخرجات
                Console.OutputEncoding = Encoding.UTF8;
                
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                
                // التحقق من المتطلبات الأساسية
                CheckBasicRequirements();
                
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

        static void CheckBasicRequirements()
        {
            // التحقق من وجود المترجم
            if (!File.Exists("ArabicCompiler.exe"))
            {
                var result = MessageBox.Show(
                    "المترجم 'ArabicCompiler.exe' غير موجود في المجلد الحالي.\n\n" +
                    "بدون المترجم، يمكنك استخدام المحرر فقط دون الترجمة.\n\n" +
                    "المسار الحالي: " + Directory.GetCurrentDirectory() + "\n\n" +
                    "هل تريد الاستمرار على أي حال؟",
                    "تحذير - المترجم غير موجود",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Warning);

                if (result == DialogResult.No)
                {
                    Environment.Exit(1);
                }
            }
        }
    }
}