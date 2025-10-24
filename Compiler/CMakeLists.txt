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

# =====================
# اختبارات باستخدام CTest
# =====================
include(CTest)
if(BUILD_TESTING)
  enable_testing()

  set(EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/../Examples)
  add_test(NAME tokens_example1
           COMMAND ArabicCompiler ${EXAMPLES_DIR}/example1.arabic --tokens)

  add_test(NAME build_all_example1
           COMMAND ArabicCompiler ${EXAMPLES_DIR}/example1.arabic --all)
  set_tests_properties(build_all_example1 PROPERTIES
    PASS_REGULAR_EXPRESSION "الترجمة تمت بنجاح")

  add_test(NAME build_all_example2
           COMMAND ArabicCompiler ${EXAMPLES_DIR}/example2.arabic --all)
  set_tests_properties(build_all_example2 PROPERTIES
    PASS_REGULAR_EXPRESSION "الترجمة تمت بنجاح")

  add_test(NAME build_all_example3
           COMMAND ArabicCompiler ${EXAMPLES_DIR}/example3.arabic --all)
  set_tests_properties(build_all_example3 PROPERTIES
    PASS_REGULAR_EXPRESSION "الترجمة تمت بنجاح")
endif()