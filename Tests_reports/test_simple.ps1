#!/usr/bin/env powershell
# Simple Test Suite for Arabic Compiler

$pass = 0
$fail = 0

# Get all Arabic example files
$files = Get-ChildItem "Examples\*.arabic" | Where-Object { $_.Length -gt 0 }

foreach ($file in $files) {
    $filename = $file.Name
    Write-Host "Testing: $filename"
    
    # Test AST generation
    & ".\Compiler\build\Release\ArabicCompiler.exe" $file.FullName --ast > $null 2>&1
    $astSuccess = $LASTEXITCODE -eq 0
    
    # Test IR generation
    & ".\Compiler\build\Release\ArabicCompiler.exe" $file.FullName --ir > $null 2>&1
    $irSuccess = $LASTEXITCODE -eq 0
    
    # Test C code generation
    & ".\Compiler\build\Release\ArabicCompiler.exe" $file.FullName --c > $null 2>&1
    $cSuccess = $LASTEXITCODE -eq 0
    
    # Test ASM generation
    & ".\Compiler\build\Release\ArabicCompiler.exe" $file.FullName --asm > $null 2>&1
    $asmSuccess = $LASTEXITCODE -eq 0
    
    if ($astSuccess -and $irSuccess -and $cSuccess -and $asmSuccess) {
        Write-Host "  PASS (AST, IR, C, ASM)" -ForegroundColor Green
        $pass++
    } else {
        Write-Host "  FAIL" -ForegroundColor Red
        if (-not $astSuccess) { Write-Host "    - AST: FAIL" }
        if (-not $irSuccess) { Write-Host "    - IR: FAIL" }
        if (-not $cSuccess) { Write-Host "    - C: FAIL" }
        if (-not $asmSuccess) { Write-Host "    - ASM: FAIL" }
        $fail++
    }
}

Write-Host ""
Write-Host "Results: PASS=$pass, FAIL=$fail, TOTAL=$($pass + $fail)"

