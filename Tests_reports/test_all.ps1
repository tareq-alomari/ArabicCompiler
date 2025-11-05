#!/usr/bin/env powershell
# Comprehensive Test Suite for Arabic Compiler

Write-Host "========================================" -ForegroundColor Yellow
Write-Host "اختبار شامل لمترجم اللغة العربية" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Yellow
Write-Host ""

$pass = 0
$fail = 0
$results = @()

# Get all Arabic example files
$files = Get-ChildItem "Examples\*.arabic" | Where-Object { $_.Length -gt 0 }

Write-Host "عدد الملفات المراد اختبارها: $($files.Count)" -ForegroundColor Cyan
Write-Host ""

foreach ($file in $files) {
    $filename = $file.Name
    Write-Host "اختبار: $filename" -ForegroundColor Cyan
    
    # Test AST generation
    $astResult = & ".\Compiler\build\Release\ArabicCompiler.exe" $file.FullName --ast 2>&1
    $astSuccess = $LASTEXITCODE -eq 0
    
    # Test IR generation
    $irResult = & ".\Compiler\build\Release\ArabicCompiler.exe" $file.FullName --ir 2>&1
    $irSuccess = $LASTEXITCODE -eq 0
    
    # Test C code generation
    $cResult = & ".\Compiler\build\Release\ArabicCompiler.exe" $file.FullName --c 2>&1
    $cSuccess = $LASTEXITCODE -eq 0
    
    # Test ASM generation
    $asmResult = & ".\Compiler\build\Release\ArabicCompiler.exe" $file.FullName --asm 2>&1
    $asmSuccess = $LASTEXITCODE -eq 0
    
    if ($astSuccess -and $irSuccess -and $cSuccess -and $asmSuccess) {
        Write-Host "  ✅ نجح (AST, IR, C, ASM)" -ForegroundColor Green
        $pass++
        $results += @{
            File = $filename
            Status = "PASS"
            AST = "✅"
            IR = "✅"
            C = "✅"
            ASM = "✅"
        }
    } else {
        Write-Host "  ❌ فشل" -ForegroundColor Red
        if (-not $astSuccess) { Write-Host "    - AST: فشل" -ForegroundColor Red }
        if (-not $irSuccess) { Write-Host "    - IR: فشل" -ForegroundColor Red }
        if (-not $cSuccess) { Write-Host "    - C: فشل" -ForegroundColor Red }
        if (-not $asmSuccess) { Write-Host "    - ASM: فشل" -ForegroundColor Red }
        $fail++
        $results += @{
            File = $filename
            Status = "FAIL"
            AST = if ($astSuccess) { "✅" } else { "❌" }
            IR = if ($irSuccess) { "✅" } else { "❌" }
            C = if ($cSuccess) { "✅" } else { "❌" }
            ASM = if ($asmSuccess) { "✅" } else { "❌" }
        }
    }
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Yellow
Write-Host "ملخص النتائج" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Yellow
Write-Host "نجح: $pass" -ForegroundColor Green
Write-Host "فشل: $fail" -ForegroundColor Red
Write-Host "الإجمالي: $($pass + $fail)" -ForegroundColor Cyan
Write-Host ""

# Display detailed results table
Write-Host "جدول النتائج التفصيلية:" -ForegroundColor Yellow
Write-Host ""
$results | Format-Table -AutoSize

