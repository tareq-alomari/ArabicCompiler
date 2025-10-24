# CMake generated Testfile for 
# Source directory: /workspace/Compiler
# Build directory: /workspace/Compiler/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(tokens_example1 "/workspace/Compiler/build/ArabicCompiler" "/workspace/Compiler/../Examples/example1.arabic" "--tokens")
set_tests_properties(tokens_example1 PROPERTIES  _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;37;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(build_all_example1 "/workspace/Compiler/build/ArabicCompiler" "/workspace/Compiler/../Examples/example1.arabic" "--all")
set_tests_properties(build_all_example1 PROPERTIES  PASS_REGULAR_EXPRESSION "الترجمة تمت بنجاح" _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;40;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(build_all_example2 "/workspace/Compiler/build/ArabicCompiler" "/workspace/Compiler/../Examples/example2.arabic" "--all")
set_tests_properties(build_all_example2 PROPERTIES  PASS_REGULAR_EXPRESSION "الترجمة تمت بنجاح" _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;45;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(build_all_example3 "/workspace/Compiler/build/ArabicCompiler" "/workspace/Compiler/../Examples/example3.arabic" "--all")
set_tests_properties(build_all_example3 PROPERTIES  PASS_REGULAR_EXPRESSION "الترجمة تمت بنجاح" _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;50;add_test;/workspace/Compiler/CMakeLists.txt;0;")
