#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
اختبار شامل لجميع أمثلة مترجم اللغة العربية
Comprehensive Test for All Arabic Compiler Examples
"""

import os
import subprocess
import sys
from pathlib import Path

# الألوان للطباعة
class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    RESET = '\033[0m'

def print_header(text):
    """طباعة رأس القسم"""
    print(f"\n{Colors.BLUE}{'='*60}")
    print(f"{text}")
    print(f"{'='*60}{Colors.RESET}\n")

def print_success(text):
    """طباعة رسالة نجاح"""
    print(f"{Colors.GREEN}✅ {text}{Colors.RESET}")

def print_error(text):
    """طباعة رسالة خطأ"""
    print(f"{Colors.RED}❌ {text}{Colors.RESET}")

def print_warning(text):
    """طباعة رسالة تحذير"""
    print(f"{Colors.YELLOW}⚠️  {text}{Colors.RESET}")

def find_compiler():
    """البحث عن المترجم"""
    possible_paths = [
        "Compiler/build/Release/ArabicCompiler.exe",
        "IDE/ArabicCompiler.exe",
        "ArabicCompiler.exe"
    ]
    
    for path in possible_paths:
        if os.path.exists(path):
            return path
    
    return None

def test_example(compiler_path, example_file):
    """اختبار ملف مثال واحد"""
    try:
        result = subprocess.run(
            [compiler_path, example_file, "--ast"],
            capture_output=True,
            text=True,
            timeout=10,
            encoding='utf-8',
            errors='ignore'
        )
        
        if result.returncode == 0:
            return True, "نجح"
        else:
            return False, result.stderr[:100] if result.stderr else "خطأ غير معروف"
    except subprocess.TimeoutExpired:
        return False, "انتهت المهلة الزمنية"
    except Exception as e:
        return False, str(e)[:100]

def main():
    """الدالة الرئيسية"""
    print_header("اختبار شامل لأمثلة مترجم اللغة العربية")
    
    # البحث عن المترجم
    compiler_path = find_compiler()
    if not compiler_path:
        print_error("لم يتم العثور على المترجم!")
        print("تأكد من بناء المشروع أولاً")
        return 1
    
    print_success(f"تم العثور على المترجم: {compiler_path}")
    
    # البحث عن الأمثلة
    examples_dir = Path("Examples")
    if not examples_dir.exists():
        print_error("مجلد الأمثلة غير موجود!")
        return 1
    
    # جمع ملفات الأمثلة
    example_files = sorted([
        f for f in examples_dir.glob("*.arabic")
        if f.name.startswith(tuple("0123456789"))
    ])
    
    if not example_files:
        print_error("لم يتم العثور على أمثلة!")
        return 1
    
    print_success(f"تم العثور على {len(example_files)} مثال")
    
    # تشغيل الاختبارات
    print_header("تشغيل الاختبارات")
    
    passed = 0
    failed = 0
    results = []
    
    for example_file in example_files:
        example_name = example_file.name
        print(f"اختبار: {example_name}...", end=" ")
        
        success, message = test_example(compiler_path, str(example_file))
        
        if success:
            print_success("نجح")
            passed += 1
            results.append((example_name, True, message))
        else:
            print_error(f"فشل - {message}")
            failed += 1
            results.append((example_name, False, message))
    
    # طباعة النتائج
    print_header("ملخص النتائج")
    
    print(f"إجمالي الاختبارات: {len(example_files)}")
    print_success(f"نجح: {passed}")
    print_error(f"فشل: {failed}")
    
    if failed > 0:
        print_warning("\nالاختبارات الفاشلة:")
        for name, success, message in results:
            if not success:
                print(f"  - {name}: {message}")
    
    # نسبة النجاح
    success_rate = (passed / len(example_files)) * 100
    print(f"\nنسبة النجاح: {success_rate:.1f}%")
    
    # الخلاصة
    print_header("الخلاصة")
    if failed == 0:
        print_success("جميع الاختبارات نجحت! 🎉")
        return 0
    else:
        print_warning(f"هناك {failed} اختبار فاشل")
        return 1

if __name__ == "__main__":
    sys.exit(main())

