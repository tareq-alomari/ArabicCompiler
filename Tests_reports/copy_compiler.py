#!/usr/bin/env python3
import shutil
import os

src = r'D:\level4\compiler\ArabicCompiler\Compiler\build\Release\ArabicCompiler.exe'
dst = r'D:\level4\compiler\ArabicCompiler\IDE\ArabicCompiler.exe'

try:
    shutil.copy(src, dst)
    print('Copy successful!')
except Exception as e:
    print(f'Error: {e}')

