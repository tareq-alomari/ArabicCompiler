$ErrorActionPreference = 'Stop'
$repoRoot = Resolve-Path "$(Split-Path -Parent $MyInvocation.MyCommand.Path)\.." | Select-Object -ExpandProperty Path
$compiler = Join-Path $repoRoot "Compiler\build\Release\ArabicCompiler.exe"
$examples = Join-Path $repoRoot "Examples"
$logs = Join-Path $repoRoot "build_tests_logs"
if (-not (Test-Path $compiler)) { Write-Error "Compiler not found: $compiler"; exit 2 }
New-Item -Path $logs -ItemType Directory -Force | Out-Null

Get-ChildItem -Path $examples -Filter *.arabic -File | ForEach-Object {
    $file = $_.FullName
    $name = $_.BaseName
    $log = Join-Path $logs "$name.log"
    $err = Join-Path $logs "$name.err"
    Write-Host "Running: $name"
    & $compiler $file --all > $log 2> $err
    if (Test-Path (Join-Path $examples "$name`_output.c")) {
        $c = Join-Path $examples "$name`_output.c"
        $exe = Join-Path $logs "$name`_run.exe"
        $gccErr = Join-Path $logs "$name`_gcc.err"
        & gcc $c -o $exe 2> $gccErr
        if ($LASTEXITCODE -eq 0) {
            $runOut = Join-Path $logs "$name`_run.out"
            $runErr = Join-Path $logs "$name`_run.err"
            & $exe > $runOut 2> $runErr
        }
    }
}
Write-Host "Done. Logs in: $logs"
