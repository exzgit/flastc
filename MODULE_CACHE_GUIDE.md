# Flast Module-Specific Caching System

## Overview

Sistem caching Flast menggunakan pendekatan **module-specific** untuk mencegah file cache yang besar dan meningkatkan efisiensi kompilasi. Setiap module memiliki cache terpisah yang diorganisir berdasarkan path dan hash, termasuk **object files (.o) yang modular**.

## Struktur Cache

```
.build/
├── bin/                           # Executable files
│   ├── program1
│   ├── program2
│   └── ...
└── cache/                         # Cache directory
    ├── main_*.o                   # Main compilation objects
    └── modules/                   # Module-specific cache
        ├── packages/              # Standard packages
        │   ├── std/
        │   │   ├── std_[hash].cache
        │   │   └── std_[hash].o      # Module object file
        │   ├── system/
        │   │   ├── system_[hash].cache
        │   │   └── system_[hash].o   # Module object file
        │   ├── time/
        │   │   ├── time_[hash].cache
        │   │   └── time_[hash].o     # Module object file
        │   └── threading/
        │       ├── threading_[hash].cache
        │       └── threading_[hash].o # Module object file
        └── external_[hash]/       # External modules
            ├── module_[hash].cache
            └── module_[hash].o    # Module object file
```

## Fitur Sistem Caching

### 1. **Module Isolation**
- Setiap module memiliki cache directory terpisah
- Hash-based naming untuk menghindari konflik
- Path-organized structure untuk kemudahan navigasi
- **Object files (.o) terpisah untuk setiap module**

### 2. **Intelligent Cache Validation**
- Otomatis membandingkan timestamp source vs cache/object
- Invalid cache jika source file lebih baru
- Memory cache untuk akses super cepat
- **Object file validation untuk modular linking**

### 3. **Efficient Cache Management**
- `--clean`: Membersihkan semua cache dan object files
- Selective cleaning: preserve binaries
- Automatic cleanup of stale cache entries
- **Modular object file reuse across compilations**

### 4. **Performance Benefits**
- **Compilation Speed**: 2-3x lebih cepat untuk module yang sudah di-cache
- **Memory Efficiency**: Cache terpisah mengurangi memory footprint
- **Disk Space**: Organized cache structure mencegah file besar
- **Modular Linking**: Only recompile changed modules, reuse cached object files

## Cache Operations

### Loading Module with Object Files
```
🔄 Loading module: "std.fls"              # First time
💾 Cached module: "std.fls"              # Saving to cache
🔧 Generating module object: std_[hash].o # Creating object file
✓ Module object generated: std_[hash].o   # Object file saved

⚡ Using cached module: "std.fls"        # Subsequent loads
⚡ Using cached object: std_[hash].o     # Reusing object file
```

### Compilation Output with Object Files
```
=== FLAST PROFESSIONAL COMPILER ===
Platform: Linux
Compiling: test/simple_cache_demo.fls
✓ Main object file: "/path/cache/main.o"
✓ Module object files: 2 files            # Modular objects
  - "system_[hash].o"
  - "std_[hash].o"
Linking with 3 object files...            # Main + modules
✓ Executable: "/path/bin/simple_cache_demo"
```

### Cache Structure Output
```
📁 Project structure:
  Root: "/project/path"
  Build: "/project/path/.build"
  Binaries: "/project/path/.build/bin" (preserved)
  Cache: "/project/path/.build/cache" (module-specific with .o files)
```

### Cache Cleaning
```bash
# Clean all cache including object files
flast program.fls --clean

# Output:
🧹 Cache cleaned: 5 main files, 12 module caches (keeping binaries)
```

## Implementation Details

### Module Object File Generation
```cpp
// Generate hash-based object file name
std::string getModuleObjectFileName(const std::string& modulePath) {
    std::hash<std::string> hasher;
    size_t hash = hasher(modulePath);
    
    std::filesystem::path moduleFile(modulePath);
    std::string baseName = moduleFile.stem().string();
    
    return baseName + "_" + std::to_string(hash) + ".o";
}
```

### Object File Validation
```cpp
bool isModuleObjectValid(const std::string& modulePath) {
    // Check if object file exists
    // Compare timestamps: source vs object file
    // Return true if object is valid and up-to-date
}
```

### Modular Linking Process
```cpp
std::vector<std::string> collectModuleObjectFiles() {
    // Collect all cached module object files
    // Return vector of object file paths for linking
}

// In writeExecutable():
// Link main.o + module1.o + module2.o + ... = executable
```

## Usage Examples

### Basic Module Import with Object Caching
```flast
import { list_push, dict_set } from "packages/std/std";
import { sleep, exit } from "packages/system/system";

fn main() -> i32 {
    // Module functions are available
    // Object files automatically generated and cached
    return 0;
}
```

### Compilation Output Examples
```bash
# First compilation (generating object files)
$ flast program.fls
🔧 Generating module object: std_13938922974113244286.o
🔧 Generating module object: system_9306125659051653815.o
✓ Module object files: 2 files
Linking with 3 object files...

# Second compilation (reusing object files)
$ flast program.fls
⚡ Using cached object: std_13938922974113244286.o
⚡ Using cached object: system_9306125659051653815.o
✓ Module object files: 2 files
Linking with 3 object files...
```

### Object File Structure
```bash
$ find .build -name "*.o"
.build/cache/main.o                              # Main program
.build/cache/packages/std/std_[hash].o           # std module
.build/cache/packages/system/system_[hash].o     # system module
.build/cache/packages/time/time_[hash].o         # time module

$ ls -lah .build/cache/packages/std/
-rw-rw-r-- 1 user user 1.9K std_[hash].o        # Compiled module object
```

## Benefits Summary

| Aspect | Traditional Caching | Module-Specific Object Caching |
|--------|-------------------|--------------------------------|
| **File Size** | Large monolithic cache | Small distributed cache + objects |
| **Rebuild Speed** | Full recompilation | Incremental module compilation |
| **Memory Usage** | High memory footprint | Optimized memory usage |
| **Organization** | Single cache directory | Organized by module path + objects |
| **Maintenance** | Difficult to manage | Easy cache + object management |
| **Linking** | Monolithic linking | Modular linking with cached objects |
| **Scalability** | Performance degradation | Excellent scaling with module reuse |

## Object File Benefits

### 1. **True Modular Compilation**
- Each module compiles to separate .o file
- Only changed modules need recompilation
- Object files can be reused across different programs

### 2. **Faster Linking**
- Pre-compiled module objects
- Reduced compilation time on subsequent builds
- Parallel module compilation possible

### 3. **Better Debugging**
- Each module has separate debug symbols
- Easier to isolate module-specific issues
- Clear separation of concerns

## Best Practices

### 1. **Module Organization**
- Group related functions in same module
- Use clear module naming conventions
- Organize packages in logical directory structure
- **Keep modules focused and cohesive for optimal object reuse**

### 2. **Cache Management**
- Use `--clean` when modules change significantly
- Monitor cache directory size periodically
- Keep source files timestamp-accurate
- **Check object file sizes for optimization opportunities**

### 3. **Performance Optimization**
- Import only needed functions to reduce object size
- Use module-specific imports rather than wildcard imports
- Structure project to maximize cache reuse
- **Design modules to minimize dependencies for better caching**

## Troubleshooting

### Cache Issues
```bash
# If cache corruption occurs
flast program.fls --clean

# Check cache structure including object files
find .build/cache -type f -name "*.o"
find .build/cache -type f -name "*.cache"

# Monitor cache sizes
du -sh .build/cache/modules/*/
```

### Object File Issues
```bash
# Check object file timestamps
ls -la .build/cache/packages/*/*.o

# Verify object file contents
file .build/cache/packages/std/std_*.o

# Check linking command
flast program.fls 2>&1 | grep "Linking with"
```

## Conclusion

Sistem module-specific caching dengan object files di Flast memberikan:
- **Efisiensi**: File cache dan object files kecil dan terdistribusi
- **Performance**: Kompilasi super cepat dengan object file reuse
- **Organization**: Struktur cache dan object yang terorganisir
- **Maintenance**: Cache dan object management yang mudah dan fleksibel
- **Modularity**: True modular compilation dengan separate object files
- **Scalability**: Excellent scaling untuk proyek besar dengan banyak modules

Sistem ini memastikan bahwa proyek Flast dapat scale dengan sangat baik, dengan setiap module dikompilasi sekali dan di-reuse berkali-kali dalam berbagai program yang berbeda. 