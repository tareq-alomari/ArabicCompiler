$ErrorActionPreference='Stop'

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path "$scriptDir\.." | Select-Object -ExpandProperty Path
$examplesDir = Join-Path $repoRoot "Examples"
$logsDir = Join-Path $repoRoot "build_tests_logs"
New-Item -Path $logsDir -ItemType Directory -Force | Out-Null

# Try locate ArabicCompiler.exe in common places
$possible = @( 
    Join-Path $repoRoot "Compiler\ArabicCompiler.exe",
    Join-Path $repoRoot "Compiler\build\Release\ArabicCompiler.exe",
    Join-Path $repoRoot "Compiler\build\ArabicCompiler.exe",
    Join-Path $repoRoot "ArabicCompiler.exe"
)
$compiler = $possible | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $compiler) {
    Write-Host "ERROR: ArabicCompiler.exe not found. Looked in: `n$($possible -join "`n")" -ForegroundColor Red
    exit 2
}
Write-Host "Using compiler: $compiler"

$summary = @()

Get-ChildItem -Path $examplesDir -Filter '*.arabic' -File | ForEach-Object {
    $src = $_.FullName
    $name = $_.BaseName
    $baseNoExt = [System.IO.Path]::GetFileNameWithoutExtension($src)
    $baseOut = Join-Path $($_.DirectoryName) ($baseNoExt + "_output")

    $logFile = Join-Path $logsDir ($baseNoExt + ".log")
    $gccErr = Join-Path $logsDir ($baseNoExt + ".gcc.err")
    $runOut = Join-Path $logsDir ($baseNoExt + ".run.out")
    $runErr = Join-Path $logsDir ($baseNoExt + ".run.err")

    $lines = @()
    $lines += "Source: $src"
    $lines += "Started: $(Get-Date -Format o)"

    try {
        Write-Host "Compiling source with ArabicCompiler: $name"
        # generate C (and other outputs if desired)
        $proc = Start-Process -FilePath $compiler -ArgumentList "`"$src`" --c --asm --ir" -NoNewWindow -PassThru -Wait -RedirectStandardOutput "$($logFile).stdout" -RedirectStandardError "$($logFile).stderr"
        $exit = $proc.ExitCode
        $lines += "ArabicCompiler exit code: $exit"
        $lines += "ArabicCompiler stdout:";
        $lines += Get-Content -ErrorAction SilentlyContinue -Path "$($logFile).stdout"
        $lines += "ArabicCompiler stderr:";
        $lines += Get-Content -ErrorAction SilentlyContinue -Path "$($logFile).stderr"

        if ($exit -ne 0) {
            $lines += "Result: compile-source-failed"
            $summary += "$name: compile-source-failed"
            $lines | Out-File -FilePath $logFile -Encoding utf8
            continue
        }

        # expect C file at $baseOut + ".c"
        $cfile = $baseOut + ".c"
        if (-not (Test-Path $cfile)) {
            $lines += "Result: c-not-generated"
            $summary += "$name: c-not-generated"
            $lines | Out-File -FilePath $logFile -Encoding utf8
            continue
        }

        # compile C with gcc
        Write-Host "Compiling generated C with gcc: $name"
        $exe = Join-Path $logsDir ($baseNoExt + ".exe")
        & gcc "${cfile}" -o "${exe}" 2> $gccErr
        if ($LASTEXITCODE -ne 0) {
            $lines += "gcc failed with exit $LASTEXITCODE"
            $lines += Get-Content -ErrorAction SilentlyContinue -Path $gccErr
            $summary += "$name: gcc-failed"
            $lines | Out-File -FilePath $logFile -Encoding utf8
            continue
        }

        # run the executable
        Write-Host "Running executable: $name"
        & $exe > $runOut 2> $runErr
        $runExit = $LASTEXITCODE
        $lines += "Program exit code: $runExit"
        $lines += "Program stdout:";
        $lines += Get-Content -ErrorAction SilentlyContinue -Path $runOut
        $lines += "Program stderr:";
        $lines += Get-Content -ErrorAction SilentlyContinue -Path $runErr

        if ($runExit -eq 0) {
            $lines += "Result: ok"
            $summary += "$name: ok"
        }
        else {
            $lines += "Result: runtime-failed"
            $summary += "$name: runtime-failed"
        }

        $lines | Out-File -FilePath $logFile -Encoding utf8
    }
    catch {
        $lines += "Exception: $_"
        $lines | Out-File -FilePath $logFile -Encoding utf8
        $summary += "$name: exception"
    }
}

# write summary
$summaryFile = Join-Path $logsDir "summary.txt"
$summary | Out-File -FilePath $summaryFile -Encoding utf8
Write-Host "Done. Logs in: $logsDir"
