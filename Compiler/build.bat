@echo off
chcp 65001 >nul
echo ===========================================
echo    مترجم اللغة العربية - بناء المشروع
echo ===========================================

if not exist "build" (
    echo إنشاء مجلد البناء...
    mkdir build
)

cd build

echo تكوين المشروع باستخدام CMake...
cmake ..

if %errorlevel% neq 0 (
    echo ❌ فشل تكوين المشروع!
    exit /b 1
)

echo بناء المشروع...
cmake --build . --config Release

if %errorlevel% equ 0 (
    echo.
    echo ✅ تم بناء المترجم بنجاح!
    echo 📁 الملف التنفيذي: build\ArabicCompiler.exe
    echo.
    echo 🚀 يمكنك الآن استخدام:
    echo   ArabicCompiler.exe examples\hello.arabic
) else (
    echo ❌ فشل بناء المشروع!
    exit /b 1
)

cd ..
pause