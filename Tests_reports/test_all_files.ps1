# سكريبت اختبار شامل لجميع ملفات الأمثلة
# Comprehensive Test Script for All Example Files

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "اختبار شامل لمترجم اللغة العربية" -ForegroundColor Cyan
Write-Host "Comprehensive Test for Arabic Compiler" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# المسار إلى المترجم
$compilerPath = ".\Compiler\build\Release\ArabicCompiler.exe"
$examplesPath = ".\Examples"

# التحقق من وجود المترجم
if (-not (Test-Path $compilerPath)) {
    Write-Host "❌ خطأ: لم يتم العثور على المترجم!" -ForegroundColor Red
    Write-Host "Error: Compiler not found!" -ForegroundColor Red
    exit 1
}

# الحصول على قائمة الملفات
$files = Get-ChildItem -Path $examplesPath -Filter "*.arabic" | Sort-Object Name

$totalFiles = $files.Count
$successCount = 0
$failureCount = 0
$results = @()

Write-Host "عدد الملفات المراد اختبارها: $totalFiles" -ForegroundColor Yellow
Write-Host "Number of files to test: $totalFiles" -ForegroundColor Yellow
Write-Host ""

# اختبار كل ملف
foreach ($file in $files) {
    $fileName = $file.Name
    $filePath = $file.FullName
    
    Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Gray
    Write-Host "اختبار: $fileName" -ForegroundColor Cyan
    Write-Host "Testing: $fileName" -ForegroundColor Cyan
    
    # تشغيل المترجم
    $output = & $compilerPath $filePath --ast 2>&1
    $exitCode = $LASTEXITCODE
    
    if ($exitCode -eq 0) {
        Write-Host "✅ نجح" -ForegroundColor Green
        Write-Host "✅ Success" -ForegroundColor Green
        $successCount++
        $results += @{
            File = $fileName
            Status = "✅ نجح"
            ExitCode = $exitCode
        }
    } else {
        Write-Host "❌ فشل (رمز الخروج: $exitCode)" -ForegroundColor Red
        Write-Host "❌ Failed (Exit Code: $exitCode)" -ForegroundColor Red
        $failureCount++
        $results += @{
            File = $fileName
            Status = "❌ فشل"
            ExitCode = $exitCode
        }
    }
}

# ملخص النتائج
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "ملخص النتائج" -ForegroundColor Cyan
Write-Host "Test Summary" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "إجمالي الملفات: $totalFiles" -ForegroundColor Yellow
Write-Host "Total Files: $totalFiles" -ForegroundColor Yellow

Write-Host "الملفات الناجحة: $successCount" -ForegroundColor Green
Write-Host "Successful Files: $successCount" -ForegroundColor Green

Write-Host "الملفات الفاشلة: $failureCount" -ForegroundColor Red
Write-Host "Failed Files: $failureCount" -ForegroundColor Red

$successRate = if ($totalFiles -gt 0) { [math]::Round(($successCount / $totalFiles) * 100, 2) } else { 0 }
Write-Host "معدل النجاح: $successRate%" -ForegroundColor Yellow
Write-Host "Success Rate: $successRate%" -ForegroundColor Yellow

Write-Host ""
Write-Host "تفاصيل النتائج:" -ForegroundColor Cyan
Write-Host "Detailed Results:" -ForegroundColor Cyan
Write-Host ""

foreach ($result in $results) {
    $status = $result.Status
    $file = $result.File
    $code = $result.ExitCode
    
    if ($code -eq 0) {
        Write-Host "$status - $file" -ForegroundColor Green
    } else {
        Write-Host "$status - $file (Code: $code)" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "انتهى الاختبار" -ForegroundColor Cyan
Write-Host "Test Complete" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# إرجاع رمز الخروج
if ($failureCount -eq 0) {
    exit 0
} else {
    exit 1
}

