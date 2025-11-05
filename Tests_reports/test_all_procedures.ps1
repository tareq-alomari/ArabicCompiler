$files = Get-ChildItem ".\Examples\*.arabic"
$passed = 0
$failed = 0

foreach ($file in $files) {
    Write-Host "Testing: $($file.Name)" -ForegroundColor Cyan
    $output = & ".\Compiler\build\Release\ArabicCompiler.exe" $file.FullName --ast 2>&1
    
    if ($output -match "✅ التحليل النحوي تم بنجاح") {
        Write-Host "  ✅ PASSED" -ForegroundColor Green
        $passed++
    } else {
        Write-Host "  ❌ FAILED" -ForegroundColor Red
        $failed++
    }
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Yellow
Write-Host "Total: $($passed + $failed) | Passed: $passed | Failed: $failed" -ForegroundColor Yellow
Write-Host "Success Rate: $(($passed / ($passed + $failed) * 100))%" -ForegroundColor Yellow

