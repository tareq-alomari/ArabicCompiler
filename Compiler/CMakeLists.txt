cmake_minimum_required(VERSION 3.10)
project(ArabicCompiler)

# الحد الأدنى من الإعدادات
set(CMAKE_CXX_STANDARD 17)

# ملفات المصدر
set(SOURCE_FILES
    src/Lexer.cpp
    src/Parser.cpp
    src/Compiler.cpp
    src/AST.cpp
    src/main.cpp
)

add_executable(ArabicCompiler ${SOURCE_FILES})

# نسخ الملف التنفيذي
add_custom_command(TARGET ArabicCompiler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ArabicCompiler> ${CMAKE_BINARY_DIR}/
)