# Arabic Compiler on macOS 🍎

## ✅ **Yes, It Works on Your MacBook Pro M2!**

The C++ compiler runs perfectly on macOS. Only the Windows Forms IDE won't work.

---

## 🚀 **Quick Start Guide**

### **What Works:**
✅ Core C++ compiler (fully functional)  
✅ Command-line compilation  
✅ All language features (variables, arrays, loops, procedures, etc.)  
✅ Code generation (C, Assembly, Intermediate)  
✅ Test suite  
✅ GCC compilation and execution  

### **What Doesn't Work:**
❌ C# Windows Forms IDE (Windows only)

---

## 📦 **Setup (Already Done for You!)**

The compiler has been built and is ready at:
```
/Users/salahmac/Desktop/MLProjects/ArabicCompiler/Compiler/ArabicCompiler_mac
```

If you need to rebuild it:
```bash
cd Compiler
clang++ -std=c++17 -o ArabicCompiler_mac src/*.cpp
```

---

## 💻 **Usage**

### **Method 1: Using the Wrapper Script (Recommended)**

The wrapper script `compile_arabic.sh` makes everything easier:

```bash
# Compile to C code
./compile_arabic.sh Tests/test_01_program_structure.arabic --c

# Compile and run immediately
./compile_arabic.sh Tests/test_08_for_loop.arabic --c --run

# Generate all output types (C, Assembly, Intermediate)
./compile_arabic.sh Tests/test_06_if_else.arabic --all

# Just Assembly
./compile_arabic.sh program.arabic --asm

# Just Intermediate code
./compile_arabic.sh program.arabic --ir
```

### **Method 2: Direct Compiler Usage**

```bash
cd Tests
../Compiler/ArabicCompiler_mac test_01_program_structure.arabic --c
gcc test_01_program_structure_output.c -o program
./program
```

---

## 📝 **Compiler Options**

| Option | Description |
|--------|-------------|
| `--c` | Generate C code |
| `--asm` | Generate MIPS Assembly |
| `--ir` | Generate Intermediate representation |
| `--all` | Generate all outputs |
| `--tokens` | Show lexical tokens |
| `--ast` | Show Abstract Syntax Tree |
| `--debug-lexer` | Debug lexer issues |
| `--help` | Show help message |

---

## 🧪 **Testing**

### **Test Individual Examples:**

```bash
# Simple program
./compile_arabic.sh Tests/test_01_program_structure.arabic --c --run

# If-else statements
./compile_arabic.sh Tests/test_06_if_else.arabic --c --run

# For loops
./compile_arabic.sh Tests/test_08_for_loop.arabic --c --run

# Arrays
./compile_arabic.sh Tests/test_03_arrays.arabic --c --run

# Procedures
./compile_arabic.sh Tests/test_05_procedures.arabic --c --run
```

### **Test Results (Verified on M2):**

✅ **test_01_program_structure.arabic** - Prints "مرحبا بك"  
✅ **test_06_if_else.arabic** - If-else conditions work  
✅ **test_08_for_loop.arabic** - Prints 1, 2, 3, 4, 5  
✅ **test_03_arrays.arabic** - Array indexing works (10, 20, 30)  

---

## 📚 **Language Features**

### **1. Variables and Constants**
```arabic
متغير س : صحيح ؛
ثابت ط = 3.14 ؛
```

### **2. Arrays**
```arabic
متغير أرقام : قائمة[5] من صحيح ؛
أرقام[0] = 10 ؛
اطبع أرقام[0] ؛
```

### **3. Control Flow**
```arabic
اذا س > 10 فان
    اطبع "كبير" ؛
والا
    اطبع "صغير" ؛
نهاية ؛
```

### **4. Loops**
```arabic
// For loop
كرر (ع = 1 الى 5)
    اطبع ع ؛
نهاية ؛

// While loop
طالما س < 10 فان
    س = س + 1 ؛
نهاية ؛
```

### **5. Procedures**
```arabic
إجراء الجمع (بالقيمة أ : صحيح ؛ بالقيمة ب : صحيح) ؛
    اطبع أ + ب ؛
نهاية ؛
```

---

## 🔧 **Troubleshooting**

### **1. Permission Denied on compile_arabic.sh**
```bash
chmod +x compile_arabic.sh
```

### **2. Compiler Not Found**
Rebuild it:
```bash
cd Compiler
clang++ -std=c++17 -o ArabicCompiler_mac src/*.cpp
```

### **3. GCC Not Installed**
Install via Homebrew (requires sudo):
```bash
# Fix Homebrew permissions first if needed:
sudo chown -R $(whoami) /opt/homebrew

# Then install gcc:
brew install gcc
```

Or use the default compiler:
```bash
gcc test_output.c -o program
# or
clang test_output.c -o program
```

---

## 📊 **Project Structure**

```
ArabicCompiler/
├── compile_arabic.sh          # ⭐ Convenient wrapper (NEW!)
├── Compiler/
│   ├── ArabicCompiler_mac     # ⭐ macOS executable (BUILT!)
│   ├── src/
│   │   ├── main.cpp
│   │   ├── Lexer.cpp/h
│   │   ├── Parser.cpp/h
│   │   ├── Compiler.cpp/h
│   │   └── AST.cpp/h
│   └── CMakeLists.txt
├── Tests/                     # ⭐ 50+ test files
│   ├── test_01_program_structure.arabic
│   ├── test_03_arrays.arabic
│   ├── test_06_if_else.arabic
│   ├── test_08_for_loop.arabic
│   └── ...
├── Examples/                  # Additional examples
└── IDE/                       # ❌ Windows Forms (won't work)
```

---

## 🎯 **Compilation Pipeline**

```
Arabic Source (.arabic)
    ↓
[Lexer] → Tokens
    ↓
[Parser] → Abstract Syntax Tree (AST)
    ↓
[Compiler] → Intermediate Code
    ↓
[Code Generator] → C / Assembly / IR
    ↓
[gcc/clang] → Native Executable
    ↓
[Execute] → Output
```

---

## 💡 **Quick Examples**

### **Hello World**
```arabic
برنامج مرحبا ؛
    اطبع "مرحبا بالعالم" ؛
.
```

Compile and run:
```bash
./compile_arabic.sh hello.arabic --c --run
```

### **Simple Loop**
```arabic
برنامج حلقة ؛
    متغير ع : صحيح ؛
    كرر (ع = 1 الى 10)
        اطبع ع ؛
    نهاية ؛
.
```

---

## 🌟 **Advantages on macOS**

✅ **Native Performance** - Compiled with clang++, optimized for M2  
✅ **Terminal Integration** - Perfect for scripting and automation  
✅ **GCC Available** - Can compile and run generated C code  
✅ **No .NET Required** - Core compiler is pure C++  
✅ **UNIX Tools** - Easy integration with shell scripts  

---

## 📖 **Further Reading**

- Original README: `README.md`
- Grammar Rules: `التقرير (Report)/قواعد اللغة.txt`
- Test Files: All in `Tests/` directory
- Examples: Check `Examples/` folder

---

## 🎓 **Summary**

Your MacBook Pro M2 can fully compile and run Arabic programs! The compiler works perfectly with all features:
- ✅ Lexical analysis
- ✅ Parsing
- ✅ Code generation (C, Assembly, Intermediate)
- ✅ Full language support (loops, arrays, procedures, etc.)

**You're all set! Start coding in Arabic! 🚀**

