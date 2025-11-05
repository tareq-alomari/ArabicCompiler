$ErrorActionPreference = 'Stop'

# Paths
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path "$scriptDir\.." | Select-Object -ExpandProperty Path
$examples = Join-Path $repoRoot "Examples"
$logs = Join-Path $repoRoot "build_tests_logs"
New-Item -Path $logs -ItemType Directory -Force | Out-Null

# Helper: write summary lines
$summary = @()

# Check gcc availability
try {
    & gcc --version > $null 2>&1
} catch {
    Write-Host "Warning: gcc not found in PATH. The script will skip compilation/execution but will still report available generated C files." -ForegroundColor Yellow
    $gccAvailable = $false
} finally {
    if ($LASTEXITCODE -eq 0) { $gccAvailable = $true }
}

Get-ChildItem -Path $examples -Filter '*_output.c' -File | ForEach-Object {
    $c = $_.FullName
    $name = $_.BaseName -replace '_output$',''
    $exe = Join-Path $logs "$name`_run.exe"
    $gccErr = Join-Path $logs "$name`_gcc.err"
    $runOut = Join-Path $logs "$name`_run.out"
    $runErr = Join-Path $logs "$name`_run.err"
    $log = Join-Path $logs "$name`.log"

    Write-Host "Processing $($_.Name) -> $name";
    $lines = @();
    $lines += "C file: $c"

    if (-not $gccAvailable) {
        $lines += "gcc not available: skipping compile/run"
        $lines | Out-File -FilePath $log -Encoding utf8
        $summary += "$name: skipped (no gcc)"
        return
    }

    # Compile (capture stderr)
    Write-Host "Compiling $name..."
    & gcc $c -o $exe 2> $gccErr
    if ($LASTEXITCODE -ne 0) {
        $lines += "gcc failed. See $gccErr"
        $lines += Get-Content -ErrorAction SilentlyContinue -Path $gccErr
        $lines | Out-File -FilePath $log -Encoding utf8
        $summary += "$name: compile-failed"
        return
    }

    # Run executable and capture stdout/stderr
    Write-Host "Running $name..."
    try {
        & $exe > $runOut 2> $runErr
        $lines += "--- stdout ---"
        $lines += Get-Content -ErrorAction SilentlyContinue -Path $runOut
        $lines += "--- stderr ---"
        $lines += Get-Content -ErrorAction SilentlyContinue -Path $runErr
        $lines | Out-File -FilePath $log -Encoding utf8
        $summary += "$name: ok"
    } catch {
        $lines += "Execution failed: $_"
        $lines | Out-File -FilePath $log -Encoding utf8
        $summary += "$name: run-failed"
    }
}

# Write summary
$summaryFile = Join-Path $logs "summary.txt"
$summary | Out-File -FilePath $summaryFile -Encoding utf8
Write-Host "Done. Logs and outputs in: $logs"