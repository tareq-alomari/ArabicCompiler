# Cross-Platform IDE Research & Recommendation
**Project:** ArabicCompiler  
**Author:** Developer  
**Date:** November 5, 2025  
**Status:** In Progress

---

## Executive Summary

**Research Goal:** Evaluate cross-platform UI frameworks to replace the current Windows Forms IDE with a modern, truly cross-platform solution that works seamlessly on Windows, macOS, and Linux.

**Time-box:** 3-4 days  
**Deliverable:** Framework recommendation with proof-of-concept validation

---

## Current State Analysis

### Existing IDE (Windows Forms / C#)

**File:** `IDE/MainForm.cs` (1,428 lines)

#### Core Features Identified:

| Feature | Implementation | Critical? |
|---------|----------------|-----------|
| **Code Editor** | `RichTextBox` with custom syntax highlighting | ✅ Essential |
| **Syntax Highlighting** | Regex-based, real-time with timer | ✅ Essential |
| **Output Console** | `RichTextBox` for compiler output | ✅ Essential |
| **File Operations** | New, Open, Save, Save As | ✅ Essential |
| **Compiler Integration** | Process invocation via `System.Diagnostics.Process` | ✅ Essential |
| **Run Executable** | Execute compiled programs, capture output | ✅ Essential |
| **Toolbar** | Action buttons for common operations | ⚠️ Important |
| **Status Bar** | Line/column info, status messages | ⚠️ Important |
| **Output Type Selector** | ComboBox for C/ASM/IR output | ⚠️ Important |
| **Settings Persistence** | JSON-based settings file | ⚠️ Nice to have |
| **Run All Examples** | Batch test runner | ⚠️ Nice to have |
| **Debug Lexer** | Special debug mode | ⚠️ Nice to have |

#### Architecture Patterns:

```csharp
// Process invocation pattern (line ~666)
ProcessStartInfo startInfo = new ProcessStartInfo
{
    FileName = compilerPath,
    Arguments = args,
    RedirectStandardOutput = true,
    RedirectStandardError = true,
    UseShellExecute = false,
    CreateNoWindow = true
};

Process process = new Process { StartInfo = startInfo };
process.OutputDataReceived += (s, e) => { /* capture output */ };
process.Start();
```

#### Technical Details:

- **Encoding Support:** Windows-1256 for Arabic text
- **RTL Support:** Right-to-left text flow for Arabic
- **Syntax Highlighting:** Debounced with 300ms timer
- **Keywords:** ~50 Arabic language keywords (برنامج، متغير، اذا، etc.)

#### Current Limitations:

- ❌ **Windows-only:** Uses `System.Windows.Forms`
- ❌ **Limited editor:** `RichTextBox` is basic compared to modern editors
- ❌ **No code completion:** No IntelliSense or suggestions
- ❌ **No debugging:** No breakpoint/step-through capability
- ❌ **Manual syntax highlighting:** Regex-based, not language-aware

---

## Framework Candidates

### 1. Avalonia UI

**Official Site:** https://avaloniaui.net/  
**License:** MIT  
**Maturity:** Production-ready (v11.x)

#### Overview:
- WPF-inspired XAML-based framework
- True cross-platform rendering (Windows, macOS, Linux, iOS, Android, WebAssembly)
- Strong community support
- Native feel on each platform

#### Initial Assessment:

**Pros (Expected):**
- ✅ Familiar to WPF/XAML developers
- ✅ Rich control library
- ✅ Good documentation
- ✅ Active development
- ✅ .NET 6+ support
- ✅ VS Code + Visual Studio support

**Cons (Expected):**
- ⚠️ Learning curve for XAML
- ⚠️ Some platform-specific quirks possible
- ⚠️ Ecosystem smaller than WPF

**Key Questions to Answer:**
- [ ] How easy is RTL/Arabic text support?
- [ ] Can we integrate AvaloniaEdit (recommended by lead)?
- [ ] Process invocation and output capture complexity?
- [ ] Build and deployment story on macOS?
- [ ] How does it feel on macOS M2?

**Editor Component Research:**
- **AvaloniaEdit**: Open-source text editor component for Avalonia
  - GitHub: https://github.com/AvaloniaUI/AvaloniaEdit
  - Based on SharpDevelop's editor
  - Syntax highlighting support
  - **Action:** Test with Arabic text in PoC

**PoC Plan:**
1. Create basic Avalonia app
2. Integrate AvaloniaEdit component
3. Test Arabic text input/display
4. Add output console (TextBox or similar)
5. Implement "Compile" button with process invocation
6. Capture and display compiler output

---

### 2. .NET MAUI

**Official Site:** https://dotnet.microsoft.com/apps/maui  
**License:** MIT  
**Maturity:** Production (v8.x)

#### Overview:
- Microsoft's official cross-platform UI framework
- Evolution of Xamarin.Forms
- Targets mobile (iOS, Android) and desktop (Windows, macOS)
- Deep .NET ecosystem integration

#### Initial Assessment:

**Pros (Expected):**
- ✅ Official Microsoft support
- ✅ Tight .NET integration
- ✅ Modern XAML patterns
- ✅ Good tooling (VS, VS Code)
- ✅ Long-term support likely

**Cons (Expected):**
- ⚠️ Primarily mobile-focused (desktop is secondary)
- ⚠️ macOS support quality unclear
- ⚠️ Potentially heavier runtime
- ⚠️ Less mature than Avalonia for desktop

**Key Questions to Answer:**
- [ ] Desktop experience vs mobile experience?
- [ ] Text editor options for desktop?
- [ ] macOS native feel quality?
- [ ] Complexity of desktop app setup?

**PoC Plan:**
1. Create desktop-targeted MAUI app
2. Same minimal IDE layout as Avalonia
3. Test process invocation
4. Evaluate macOS experience

---

### 3. Web-Based Approach (Optional)

#### Option A: Blazor Hybrid

**Technology:** Blazor WebView hosted in native app

**Pros:**
- ✅ Web tech stack (HTML/CSS/JS + C#)
- ✅ Monaco Editor available (VS Code's editor)
- ✅ Can reuse web components
- ✅ Ultimate portability

**Cons:**
- ❌ Not truly native feel
- ❌ Performance overhead
- ❌ File system access more complex
- ❌ Different programming model

#### Option B: Electron + Blazor/Web UI

**Technology:** Electron shell + web frontend

**Pros:**
- ✅ Proven pattern (VS Code, Atom, etc.)
- ✅ Excellent editor options (Monaco)
- ✅ Rich ecosystem

**Cons:**
- ❌ Large bundle size
- ❌ Higher memory usage
- ❌ Not .NET native
- ❌ Requires JavaScript bridge

**Decision:** Will evaluate only if Avalonia and MAUI prove insufficient

---

## Proof of Concept Requirements

### Minimal Viable PoC

Each PoC must demonstrate:

1. **Basic Layout:**
   ```
   ┌─────────────────────────────────┐
   │  Toolbar: [Compile] [Run]       │
   ├─────────────────────────────────┤
   │                                 │
   │   Code Editor Area              │
   │   (Multi-line text input)       │
   │                                 │
   ├─────────────────────────────────┤
   │   Output Console                │
   │   (Read-only output)            │
   └─────────────────────────────────┘
   ```

2. **Core Functionality:**
   - Write Arabic text in editor
   - Click "Compile" button
   - Execute: `ArabicCompiler <temp_file> --c`
   - Capture stdout/stderr
   - Display in output console

3. **Success Criteria:**
   - Runs on macOS without errors
   - Can execute external process
   - Can display Arabic text correctly
   - Compile time < 30 seconds for Hello World

---

## Evaluation Criteria

### Lead Developer Guidance (Key Priorities):

1. **Developer Velocity** ⚡
   - Smooth learning curve
   - High-quality tooling
   - Excellent documentation
   - Fast iteration cycles

2. **macOS Native Experience** 🍎
   - Respects macOS conventions
   - Crisp text rendering
   - Snappy performance
   - Feels "at home" on M2

3. **Code Editor Component** 📝
   - Critical: Good editor with Arabic support
   - Known option: [AvaloniaEdit](https://github.com/AvaloniaUI/AvaloniaEdit) for Avalonia
   - This could be a major deciding factor

### Evaluation Matrix

| Criterion | Weight | Avalonia | MAUI | Web-Based | Winner |
|-----------|--------|----------|------|-----------|--------|
| **Developer Velocity** | 25% | TBD | TBD | TBD | - |
| **macOS Native Feel** | 25% | TBD | TBD | TBD | - |
| **Arabic/RTL Support** | 20% | TBD | TBD | TBD | - |
| **Editor Component Quality** | 15% | TBD | TBD | TBD | - |
| **Community & Ecosystem** | 10% | TBD | TBD | TBD | - |
| **Performance** | 5% | TBD | TBD | TBD | - |
| **Total Score** | 100% | - | - | - | - |

---

## Timeline

### Day 1 (Today) - Baseline & Setup ✅
- [x] Analyze current IDE requirements
- [x] Create research document framework
- [x] Install .NET 9 SDK (9.0.306)
- [x] Get approval from lead developer
- [ ] Install MAUI workload (manual: `sudo dotnet workload install maui`)
- [ ] Install Avalonia templates (manual: `dotnet new install Avalonia.Templates`)

### Day 2 - Avalonia PoC
- [ ] Create Avalonia project
- [ ] Build basic UI layout
- [ ] Implement process invocation
- [ ] Test on macOS
- [ ] Document findings

### Day 3 - MAUI PoC
- [ ] Create MAUI project
- [ ] Build basic UI layout
- [ ] Implement process invocation
- [ ] Test on macOS
- [ ] Document findings

### Day 4 - Analysis & Recommendation
- [ ] Complete comparison table
- [ ] Write recommendation section
- [ ] Outline migration strategy
- [ ] Present to lead developer

---

## Initial Observations

### System Capabilities (macOS M2)

**Verified:**
- ❌ .NET SDK **not installed** (needs installation)
- ✅ VS Code available
- ✅ Xcode Command Line Tools installed (clang++ 17.0.0)
- ✅ CMake installed (4.1.2)
- ✅ Arabic language support confirmed
- ✅ Can compile and run C++ compiler
- ✅ Homebrew package manager working

**Installation Requirements:**
- [ ] .NET 8 SDK (latest LTS)
- [ ] .NET MAUI workload
- [ ] Avalonia templates
- [ ] Required NuGet packages

**Installation Command:**
```bash
# Install .NET SDK via Homebrew
brew install --cask dotnet-sdk

# After installation, install workloads
dotnet workload install maui

# Install Avalonia templates
dotnet new install Avalonia.Templates
```

---

## Notes & Insights

### Migration Considerations

**From Windows Forms to Modern Framework:**

1. **Syntax Highlighting:**
   - Current: Regex-based, manual
   - Better: Language-aware editor component
   - Options: AvaloniaEdit, Monaco (web), TextMateSharp

2. **Process Management:**
   - Current pattern works well
   - Should be portable to any framework
   - Key: `System.Diagnostics.Process` is cross-platform

3. **Settings:**
   - Current: JSON file
   - Should work unchanged
   - Consider: platform-specific locations

4. **Arabic/RTL:**
   - Critical requirement
   - Must test thoroughly in PoC
   - May need framework-specific handling

---

## Resources

### Documentation Links

- **Avalonia:**
  - Docs: https://docs.avaloniaui.net/
  - GitHub: https://github.com/AvaloniaUI/Avalonia
  - Samples: https://github.com/AvaloniaUI/Avalonia.Samples

- **.NET MAUI:**
  - Docs: https://learn.microsoft.com/dotnet/maui/
  - GitHub: https://github.com/dotnet/maui
  - Samples: https://github.com/dotnet/maui-samples

- **Community:**
  - Avalonia Discord/Gitter
  - .NET Discord

---

## Recommendation

### [To be completed after PoC evaluation]

**Chosen Framework:** TBD

**Reasoning:** TBD

**Migration Path:** TBD

**Timeline Estimate:** TBD

---

## Appendix

### A. Current IDE Feature Breakdown

*[Detailed feature inventory will go here]*

### B. Code Samples

*[Key code snippets from PoCs will go here]*

### C. Screenshots

*[UI comparison screenshots will go here]*

---

**Document Status:** Phase A Complete - Moving to Phase B  
**Last Updated:** November 5, 2025  
**Next Update:** After Avalonia PoC (Day 2)

