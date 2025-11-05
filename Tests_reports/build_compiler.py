#!/usr/bin/env python3
import subprocess
import os
import sys

os.chdir(r'D:\level4\compiler\ArabicCompiler')

print("Building Arabic Compiler...")
result = subprocess.run(
    ['cmake', '--build', r'Compiler\build', '--config', 'Release'],
    capture_output=False
)

if result.returncode == 0:
    print("\n✅ Build successful!")
    sys.exit(0)
else:
    print("\n❌ Build failed!")
    sys.exit(1)

