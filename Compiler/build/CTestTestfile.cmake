# CMake generated Testfile for 
# Source directory: /workspace/Compiler
# Build directory: /workspace/Compiler/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(example1_ir "/usr/bin/cmake" "-DARG_BIN=/workspace/Compiler/build/ArabicCompiler" "-DINPUT=/workspace/Compiler/../Examples/example1.arabic" "-DGEN_IR=ON" "-P" "/workspace/Compiler/tests/RunOne.cmake")
set_tests_properties(example1_ir PROPERTIES  _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;35;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(example1_c "/usr/bin/cmake" "-DARG_BIN=/workspace/Compiler/build/ArabicCompiler" "-DINPUT=/workspace/Compiler/../Examples/example1.arabic" "-DGEN_C=ON" "-P" "/workspace/Compiler/tests/RunOne.cmake")
set_tests_properties(example1_c PROPERTIES  _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;42;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(example2_ir "/usr/bin/cmake" "-DARG_BIN=/workspace/Compiler/build/ArabicCompiler" "-DINPUT=/workspace/Compiler/../Examples/example2.arabic" "-DGEN_IR=ON" "-P" "/workspace/Compiler/tests/RunOne.cmake")
set_tests_properties(example2_ir PROPERTIES  _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;49;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(example2_asm "/usr/bin/cmake" "-DARG_BIN=/workspace/Compiler/build/ArabicCompiler" "-DINPUT=/workspace/Compiler/../Examples/example2.arabic" "-DGEN_ASM=ON" "-P" "/workspace/Compiler/tests/RunOne.cmake")
set_tests_properties(example2_asm PROPERTIES  _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;56;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(example3_ir "/usr/bin/cmake" "-DARG_BIN=/workspace/Compiler/build/ArabicCompiler" "-DINPUT=/workspace/Compiler/../Examples/example3.arabic" "-DGEN_IR=ON" "-P" "/workspace/Compiler/tests/RunOne.cmake")
set_tests_properties(example3_ir PROPERTIES  _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;63;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(example3_asm "/usr/bin/cmake" "-DARG_BIN=/workspace/Compiler/build/ArabicCompiler" "-DINPUT=/workspace/Compiler/../Examples/example3.arabic" "-DGEN_ASM=ON" "-P" "/workspace/Compiler/tests/RunOne.cmake")
set_tests_properties(example3_asm PROPERTIES  _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;70;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(tokens_example1 "/workspace/Compiler/build/ArabicCompiler" "/workspace/Compiler/../Examples/example1.arabic" "--tokens")
set_tests_properties(tokens_example1 PROPERTIES  _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;86;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(build_all_example1 "/workspace/Compiler/build/ArabicCompiler" "/workspace/Compiler/../Examples/example1.arabic" "--all")
set_tests_properties(build_all_example1 PROPERTIES  PASS_REGULAR_EXPRESSION "الترجمة تمت بنجاح" _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;89;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(build_all_example2 "/workspace/Compiler/build/ArabicCompiler" "/workspace/Compiler/../Examples/example2.arabic" "--all")
set_tests_properties(build_all_example2 PROPERTIES  PASS_REGULAR_EXPRESSION "الترجمة تمت بنجاح" _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;94;add_test;/workspace/Compiler/CMakeLists.txt;0;")
add_test(build_all_example3 "/workspace/Compiler/build/ArabicCompiler" "/workspace/Compiler/../Examples/example3.arabic" "--all")
set_tests_properties(build_all_example3 PROPERTIES  PASS_REGULAR_EXPRESSION "الترجمة تمت بنجاح" _BACKTRACE_TRIPLES "/workspace/Compiler/CMakeLists.txt;99;add_test;/workspace/Compiler/CMakeLists.txt;0;")
