#!/usr/bin/env python3
import os
import subprocess
import glob

os.chdir(r'D:\level4\compiler\ArabicCompiler')

files = sorted(glob.glob(r'Examples\*.arabic'))
passed = 0
failed = 0

print("=" * 60)
print("Testing Arabic Compiler with Procedures Support")
print("=" * 60)

for file in files:
    filename = os.path.basename(file)
    print(f"\nTesting: {filename}")
    try:
        result = subprocess.run(
            [r'.\Compiler\build\Release\ArabicCompiler.exe', file, '--ast'],
            capture_output=True,
            text=True,
            timeout=10
        )
        
        output = result.stdout + result.stderr
        if 'success' in output.lower() or result.returncode == 0:
            print("  PASSED")
            passed += 1
        else:
            print("  FAILED")
            failed += 1
    except Exception as e:
        print(f"  ERROR: {e}")
        failed += 1

print("\n" + "=" * 60)
print(f"Total: {passed + failed} | Passed: {passed} | Failed: {failed}")
if passed + failed > 0:
    print(f"Success Rate: {(passed / (passed + failed) * 100):.1f}%")
print("=" * 60)

