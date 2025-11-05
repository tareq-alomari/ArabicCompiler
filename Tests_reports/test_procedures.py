#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import subprocess
import glob

os.chdir(r'D:\level4\compiler\ArabicCompiler')

files = glob.glob(r'Examples\*.arabic')
passed = 0
failed = 0

print("=" * 50)
print("Testing Arabic Compiler with Procedures")
print("=" * 50)

for file in sorted(files):
    print(f"\nTesting: {os.path.basename(file)}")
    try:
        result = subprocess.run(
            [r'.\Compiler\build\Release\ArabicCompiler.exe', file, '--ast'],
            capture_output=True,
            text=True,
            timeout=10
        )
        
        if '✅ التحليل النحوي تم بنجاح' in result.stdout or '✅ التحليل النحوي تم بنجاح' in result.stderr:
            print("  ✅ PASSED")
            passed += 1
        else:
            print("  ❌ FAILED")
            failed += 1
    except Exception as e:
        print(f"  ❌ ERROR: {e}")
        failed += 1

print("\n" + "=" * 50)
print(f"Total: {passed + failed} | Passed: {passed} | Failed: {failed}")
print(f"Success Rate: {(passed / (passed + failed) * 100):.1f}%")
print("=" * 50)

