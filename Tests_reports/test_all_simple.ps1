# Simple Test Script for All Example Files

Write-Host "========================================"
Write-Host "Testing Arabic Compiler"
Write-Host "========================================"
Write-Host ""

# Path to compiler
$compilerPath = ".\Compiler\build\Release\ArabicCompiler.exe"
$examplesPath = ".\Examples"

# Check if compiler exists
if (-not (Test-Path $compilerPath)) {
    Write-Host "ERROR: Compiler not found!"
    exit 1
}

# Get list of files
$files = Get-ChildItem -Path $examplesPath -Filter "*.arabic" | Sort-Object Name

$totalFiles = $files.Count
$successCount = 0
$failureCount = 0
$results = @()

Write-Host "Total files to test: $totalFiles"
Write-Host ""

# Test each file
foreach ($file in $files) {
    $fileName = $file.Name
    $filePath = $file.FullName
    
    Write-Host "Testing: $fileName" -ForegroundColor Cyan
    
    # Run compiler
    $output = & $compilerPath $filePath --ast 2>&1
    $exitCode = $LASTEXITCODE
    
    if ($exitCode -eq 0) {
        Write-Host "  [OK] Success" -ForegroundColor Green
        $successCount++
        $results += @{
            File = $fileName
            Status = "OK"
            ExitCode = $exitCode
        }
    } else {
        Write-Host "  [FAIL] Exit Code: $exitCode" -ForegroundColor Red
        $failureCount++
        $results += @{
            File = $fileName
            Status = "FAIL"
            ExitCode = $exitCode
        }
    }
}

# Summary
Write-Host ""
Write-Host "========================================"
Write-Host "Test Summary"
Write-Host "========================================"
Write-Host ""

Write-Host "Total Files: $totalFiles"
Write-Host "Successful: $successCount" -ForegroundColor Green
Write-Host "Failed: $failureCount" -ForegroundColor Red

$successRate = if ($totalFiles -gt 0) { [math]::Round(($successCount / $totalFiles) * 100, 2) } else { 0 }
Write-Host "Success Rate: $successRate%"

Write-Host ""
Write-Host "Detailed Results:"
Write-Host ""

foreach ($result in $results) {
    $status = $result.Status
    $file = $result.File
    $code = $result.ExitCode
    
    if ($code -eq 0) {
        Write-Host "[OK] $file" -ForegroundColor Green
    } else {
        Write-Host "[FAIL] $file (Code: $code)" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "========================================"
Write-Host "Test Complete"
Write-Host "========================================"

# Return exit code
if ($failureCount -eq 0) {
    exit 0
} else {
    exit 1
}

