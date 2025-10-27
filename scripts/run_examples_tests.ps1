$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path "$repoRoot\.." | Select-Object -ExpandProperty Path
$compiler = Join-Path $repoRoot "Compiler\build\Release\ArabicCompiler.exe"
$examplesDir = Join-Path $repoRoot "Examples"
$tempDir = Join-Path $repoRoot "build_tests_expected"
$logsDir = Join-Path $repoRoot "build_tests_logs_2" # The one change I'm keeping

Write-Host "Repo root: $repoRoot"
Write-Host "Compiler: $compiler"
Write-Host "Examples: $examplesDir"

if (-not (Test-Path $compiler)) {
    Write-Error "Compiler executable not found at $compiler"
    exit 2
}

# Prepare folders
if (Test-Path $tempDir) { Remove-Item -Recurse -Force $tempDir }
if (Test-Path $logsDir) { Remove-Item -Recurse -Force $logsDir }
New-Item -ItemType Directory -Force -Path $tempDir | Out-Null
New-Item -ItemType Directory -Force -Path $logsDir | Out-Null

# Detect gcc
$gccPath = $null
try {
    $gccVersion = & gcc --version 2>$null
    if ($LASTEXITCODE -eq 0) { $gccPath = "gcc" }
} catch { }

Write-Host "GCC available: $([bool]$gccPath)"

$summary = @()

# Back up expected outputs (if present)
Get-ChildItem -Path $examplesDir -Filter *.arabic -File | ForEach-Object {
    $arabic = $_.FullName
    $base = [System.IO.Path]::GetFileNameWithoutExtension($_.Name)
    $expectedFiles = @("${base}_output.c","${base}_output.asm","${base}_output_intermediate.txt")
    foreach ($ef in $expectedFiles) {
        $src = Join-Path $examplesDir $ef
        if (Test-Path $src) {
            Copy-Item -Force -Path $src -Destination (Join-Path $tempDir $ef)
        }
    }
}

# Run tests
Get-ChildItem -Path $examplesDir -Filter *.arabic -File | ForEach-Object {
    $arabic = $_.FullName
    $base = [System.IO.Path]::GetFileNameWithoutExtension($_.Name)
    Write-Host "\n=== Testing: $($_.Name) ==="

    $logFile = Join-Path $logsDir "${base}.log"
    $errFile = Join-Path $logsDir "${base}.err"

    # Run compiler with all outputs
    Write-Host "Running compiler..."
    & $compiler $arabic --all > $logFile 2> $errFile
    $exit = $LASTEXITCODE

    $out = Get-Content -Raw $logFile
    $err = Get-Content -Raw $errFile

    if ($exit -ne 0) {
        Write-Host "[FAIL] Compiler exited with code $exit"
        Write-Host $err
        $summary += [pscustomobject]@{Example=$_.Name; Result="CompilerError"; Details=$err}
        return
    }

    Write-Host "[OK] Compiler returned 0"

    # Compare generated files with backups (if backups existed)
    $differences = @()
    $expectedFiles = @("${base}_output.c","${base}_output.asm","${base}_output_intermediate.txt")
    foreach ($ef in $expectedFiles) {
        $gen = Join-Path $examplesDir $ef
        $bak = Join-Path $tempDir $ef
        if (Test-Path $bak) {
            if (-not (Test-Path $gen)) {
                $differences += ($ef + ': MISSING')
            } else {
                $left = Get-Content -Raw $bak
                $right = Get-Content -Raw $gen
                if ($left -ne $right) {
                    $diff = Compare-Object -ReferenceObject ($left -split "\r?\n") -DifferenceObject ($right -split "\r?\n") -SyncWindow 0
                    $differences += ($ef + ': DIFFER')
                    $differences += $diff | Out-String
                } else {
                    $differences += ($ef + ': MATCH')
                }
            }
        } else {
            if (Test-Path $gen) {
                $differences += ($ef + ': GENERATED (no baseline)')
            } else {
                $differences += ($ef + ': NOT GENERATED')
            }
        }
    }

    # Try to compile and run C if available
    $runOutput = $null
    $cFile = Join-Path $examplesDir "${base}_output.c"
    if ($gccPath -and (Test-Path $cFile)) {
        $exePath = Join-Path $logsDir "${base}_run.exe"
        Write-Host "Compiling generated C with gcc..."
        & gcc "$cFile" -o "$exePath" 2> "$errFile"
        if ($LASTEXITCODE -ne 0) {
            $compileErr = Get-Content -Raw $errFile
            Write-Host "gcc failed:\n$compileErr"
            $summary += [pscustomobject]@{Example=$_.Name; Result="GccFail"; Details=$compileErr}
        } else {
            Write-Host "Running executable..."
            $runOutput = & "$exePath"
            Write-Host "---- Program output ----"
            Write-Host $runOutput
            Write-Host "------------------------"
            $summary += [pscustomobject]@{Example=$_.Name; Result="OkRun"; Details=$runOutput}
        }
    } else {
        $summary += [pscustomobject]@{Example=$_.Name; Result="CompiledOnly"; Details=(($differences -join "\n"))}
    }

    if ($differences.Count -gt 0) {
        Write-Host "Differences:"
        $differences | ForEach-Object { Write-Host "  $_" }
    }
}

# Print summary
Write-Host "\n=== Summary ==="
$summary | Format-Table -AutoSize

# Cleanup: restore backups
Get-ChildItem -Path $tempDir -File | ForEach-Object {
    $dest = Join-Path $examplesDir $_.Name
    Copy-Item -Force -Path $_.FullName -Destination $dest
}

Write-Host "Backups restored to Examples/ (cleanup)"

exit 0