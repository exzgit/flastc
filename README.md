# FLAST Programming Language

> **FLAST** - Bahasa pemrograman yang dirancang untuk menjadi lebih manusiawi, intuitif, dan produktif.

## 🎯 Visi

Di tengah dominasi bahasa-bahasa pemrograman besar yang lahir dari dekade sebelumnya, FLAST berdiri sebagai inovator yang melihat celah—bahwa developer saat ini butuh sesuatu yang lebih manusiawi.

**Bukan bahasa yang terlihat seperti mesin.**  
**Bukan bahasa yang dipenuhi tanda kurung, titik koma, dan hierarki template yang repetitif.**

FLAST ingin membuat bahasa yang:
- ✅ **Familiar**, tapi tidak generik
- ✅ **Intuitif** seperti Python, tetapi tidak menjiplak
- ✅ **Struktural** seperti C#, tapi tetap fleksibel
- ✅ **Rapi** seperti Go, tetapi tidak kaku

## 🚀 Fitur Utama

### 1. **Sintaks yang Mengalir**
```flast
// Import yang sederhana
use std.datetime

// Fungsi yang bersih
fn sayHello(name: str) {
    print "Hello, " + name
}

// Variabel dengan inferensi tipe
let user = getUser("admin")

// Control flow yang natural
if user.role == "admin" {
    log "Welcome, admin"
}
```

### 2. **Sistem Error Handling yang Cerdas**
```
error[E001]: Unexpected token 'import'
  --> test.fls:3:9
  use sys import
          ^^^^^^
help: Did you mean 'use'? In FLAST, we use 'use' for importing modules.

hints:
  • FLAST uses 'use' instead of 'import' for module imports
  • Available modules: std, system, threading, time
  • Use 'use std.datetime' to import datetime functionality
```

### 3. **Type System yang Fleksibel**
```flast
// Auto type inference
let number = 42        // i32
let text = "hello"     // str
let flag = true        // bool

// Explicit types when needed
let precise: f64 = 3.14159
let array: [i32] = [1, 2, 3, 4, 5]
```

### 4. **Module System yang Efisien**
```flast
// Import specific functionality
use std.datetime
use system.io

// Import with alias
use std.datetime as dt

// Import multiple items
use std { datetime, io, math }
```

## 📦 Instalasi

### Prerequisites
- CMake 3.22+
- LLVM 14+
- C++17 compatible compiler
- Make or Ninja

### Build Instructions
```bash
# Clone repository
git clone https://github.com/flastdev/flast.git
cd flast

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j$(nproc)

# Test the compiler
./flast --version
```

## 🛠️ Penggunaan

### Basic Compilation
```bash
# Compile a simple program
./flast program.fls

# Release build
./flast program.fls --release

# Custom output name
./flast program.fls -o myapp
```

### Debugging and Analysis
```bash
# Show tokens
./flast program.fls --tokens

# Show AST
./flast program.fls --ast

# Show LLVM IR
./flast program.fls --ir
```

### Error Handling Options
```bash
# Treat warnings as errors
./flast program.fls --warnings-as-errors

# Disable colored output
./flast program.fls --no-colors

# Verbose error information
./flast program.fls --verbose
```

## 📚 Contoh Program

### Hello World
```flast
use std.io

fn main() {
    println "Hello, FLAST World!"
}
```

### Simple Calculator
```flast
use std.io

fn add(a: i32, b: i32) -> i32 {
    return a + b
}

fn main() {
    let x = 10
    let y = 20
    let result = add(x, y)
    println "Result: " + result.to_string()
}
```

### Class Example
```flast
pub class Calculator {
    pub let result: i32 = 0
    
    pub fn add(value: i32) -> self {
        self.result += value
        return self
    }
    
    pub fn getResult() -> i32 {
        return self.result
    }
}

fn main() {
    let calc = Calculator()
    calc.add(10).add(20)
    println "Result: " + calc.getResult().to_string()
}
```

## 🏗️ Arsitektur

### Compiler Pipeline
```
Source Code (.fls)
    ↓
Lexer (Tokenization)
    ↓
Parser (AST Generation)
    ↓
Semantic Analyzer
    ↓
Code Generator (LLVM IR)
    ↓
Executable
```

### Project Structure
```
flast/
├── src/                    # Source code
│   ├── main.cpp           # Main compiler entry point
│   ├── Lexer.cpp/h        # Tokenization
│   ├── Parser.cpp/h       # Syntax analysis
│   ├── AST.h              # Abstract Syntax Tree
│   ├── CodeGen.cpp/h      # LLVM code generation
│   └── ErrorHandler.cpp/h # Error handling system
├── packages/              # Standard library modules
│   ├── std/              # Standard library
│   ├── system/           # System utilities
│   ├── threading/        # Concurrency support
│   └── time/             # Time and date utilities
├── test/                 # Test files
├── build-scripts/        # Build and test scripts
└── docs/                 # Documentation
```

## 🎨 Filosofi Desain

### 1. **Kemanusiaan di Atas Mesin**
- Error messages yang informatif dan membantu
- Sintaks yang natural dan mudah dibaca
- Dokumentasi yang jelas dan contoh yang praktis

### 2. **Kesederhanaan yang Bijaksana**
- Tidak ada boilerplate yang tidak perlu
- Type inference yang cerdas
- Default values yang masuk akal

### 3. **Kekuatan yang Terkontrol**
- Memory safety tanpa garbage collector
- Concurrency yang aman
- Performance yang dapat diprediksi

### 4. **Ekosistem yang Terintegrasi**
- Package manager yang sederhana
- Build system yang cepat
- Tooling yang produktif

## 🔧 Development

### Running Tests
```bash
# Run error handling demo
./build-scripts/test_errors.sh

# Test specific file
./flast test/simple_error.fls
```

### Contributing
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

### Code Style
- Follow C++17 standards
- Use meaningful variable names
- Add comments for complex logic
- Keep functions small and focused

## 📖 Dokumentasi

- [Error Handling Guide](ERROR_HANDLING_GUIDE.md) - Panduan lengkap sistem error handling
- [Vision Document](VISION_ERROR_HANDLING.md) - Filosofi dan visi error handling
- [Packages Guide](PACKAGES_README.md) - Dokumentasi sistem modul
- [Module Cache Guide](MODULE_CACHE_GUIDE.md) - Optimasi cache modul

## 🎯 Roadmap

### Phase 1: Foundation ✅
- [x] Basic compiler infrastructure
- [x] Lexer and parser
- [x] LLVM code generation
- [x] Error handling system
- [x] Module system

### Phase 2: Enhancement 🚧
- [ ] Standard library expansion
- [ ] Package manager
- [ ] IDE integration
- [ ] Performance optimizations
- [ ] Advanced type system features

### Phase 3: Ecosystem 🔮
- [ ] Web framework
- [ ] Database ORM
- [ ] GUI framework
- [ ] Cloud deployment tools
- [ ] Machine learning libraries

## 🤝 Kontribusi

Kami menyambut kontribusi dari komunitas! Beberapa area yang bisa dikontribusikan:

- **Standard Library**: Menambahkan fungsi dan modul baru
- **Error Handling**: Meningkatkan pesan error dan suggestions
- **Documentation**: Menulis tutorial dan dokumentasi
- **Testing**: Menambahkan test cases
- **Tooling**: IDE plugins, linters, formatters

## 📄 Lisensi

FLAST is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## 📞 Kontak

- **Email**: officialbangezz@gmail.com
- **GitHub**: [flastdev](https://github.com/flastdev)
- **Discord**: [FLAST Community](https://discord.gg/flast)

---

**FLAST** - Making programming more human, one line at a time. 🚀
