cmake_minimum_required(VERSION 3.10)
project(ArabicCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# إعدادات للتعامل مع Unicode
if(MSVC)
    add_compile_options("/utf-8")
else()
    add_compile_options("-std=c++17")
endif()

set(SOURCE_FILES
    src/Lexer.cpp
    src/Parser.cpp
    src/Compiler.cpp
    src/main.cpp
)

add_executable(ArabicCompiler ${SOURCE_FILES})

# نسخ الملفات بعد البناء
add_custom_command(TARGET ArabicCompiler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ArabicCompiler> ${CMAKE_BINARY_DIR}/ArabicCompiler.exe
    COMMENT "نسخ الملف القابل للتنفيذ"
)