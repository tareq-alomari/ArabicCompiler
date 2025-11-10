cmake_minimum_required(VERSION 3.10)
project(ArabicCompiler VERSION 1.0.0 LANGUAGES CXX)

# الحد الأدنى من الإعدادات
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# تحسين الرسائل
message(STATUS "Building ArabicCompiler v${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# ملفات المصدر
set(SOURCE_FILES
    src/Lexer.cpp
    src/Parser.cpp
    src/Compiler.cpp
    src/AST.cpp
    src/main.cpp
)

# إنشاء الملف التنفيذي
add_executable(ArabicCompiler ${SOURCE_FILES})

# تحديد مجلد الإخراج ليكون في build/ مباشرة
set_target_properties(ArabicCompiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# رسالة نجاح بعد البناء
add_custom_command(TARGET ArabicCompiler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "✅ Build successful: $<TARGET_FILE:ArabicCompiler>"
)