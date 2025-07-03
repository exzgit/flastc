# Flast Package System

Flast menyediakan sistem package dependencies yang dapat di-import untuk memperluas fungsionalitas program.

## Available Packages

### 1. Standard Package (`std`)
**File:** `packages/std/std.fls`  
**Import:** `import { function_name } from "packages/std/std";`

Menyediakan fungsi-fungsi standard untuk:
- **List operations:** `list_push`, `list_pop`, `list_contains`, `list_reverse`, `list_sort`
- **Dictionary operations:** `dict_set`, `dict_get`, `dict_has`, `dict_keys`, `dict_values`
- **String utilities:** `str_length`, `str_contains`, `str_upper`, `str_lower`, `str_trim`
- **Math utilities:** `math_abs`, `math_max`, `math_min`, `math_sqrt`
- **Type conversion:** `to_string`, `parse_int`, `parse_float`
- **Range utilities:** `range`

**Contoh penggunaan:**
```flast
import { list_reverse, math_sqrt, str_upper } from "packages/std/std";

fn main() -> i32 {
    let numbers: List<i64> = [1, 2, 3, 4, 5];
    let reversed = list_reverse(numbers);
    
    let sqrt_result = math_sqrt(16.0);
    let uppercase = str_upper("hello");
    
    return 0;
}
```

### 2. System Package (`system`)
**File:** `packages/system/system.fls`  
**Import:** `import { function_name } from "packages/system/system";`

Menyediakan operasi sistem:
- **Process control:** `exit`, `sleep`, `sleep_seconds`
- **Environment:** `get_env`, `set_env`, `get_username`, `get_home_dir`
- **Command execution:** `execute`, `execute_silent`
- **Power management:** `shutdown`, `restart`, `shutdown_in`, `restart_in`, `cancel_shutdown`
- **File system:** `file_exists`, `is_directory`, `is_file`, `create_directory`, `remove_file`

**Contoh penggunaan:**
```flast
import { exit, sleep, get_username, execute } from "packages/system/system";

fn main() -> i32 {
    let user = get_username();
    println("Hello " + user);
    
    execute("ls -la");
    sleep(1000); // Sleep 1 second
    
    exit(0);
}
```

### 3. Time Package (`time`)
**File:** `packages/time/time.fls`  
**Import:** `import { function_name } from "packages/time/time";`

Menyediakan operasi waktu dan tanggal:
- **Current time:** `now`, `now_millis`, `current_datetime`, `current_time_string`
- **DateTime creation:** `from_timestamp`, `create_datetime`
- **Formatting:** `format_datetime`, `current_date_string`, `current_time_only_string`
- **Date arithmetic:** `add_days`, `add_hours`, `add_minutes`
- **Utilities:** `diff_seconds`, `diff_days`, `is_leap_year`, `days_in_month`

**Contoh penggunaan:**
```flast
import { current_time_string, now, create_datetime, format_datetime } from "packages/time/time";

fn main() -> i32 {
    let current = current_time_string();
    println("Current time: " + current);
    
    let dt = create_datetime(2024, 12, 25, 10, 30, 0);
    let formatted = format_datetime(dt, "YYYY-MM-DD HH:mm:ss");
    println("Christmas: " + formatted);
    
    return 0;
}
```

### 4. Threading Package (`threading`)
**File:** `packages/threading/threading.fls`  
**Import:** `import { function_name } from "packages/threading/threading";`

Menyediakan operasi multi-threading:
- **Thread management:** `thread_create`, `thread_join`, `thread_detach`, `thread_current_id`
- **Synchronization:** `mutex_create`, `mutex_lock`, `mutex_unlock`, `mutex_destroy`
- **Atomic operations:** `atomic_increment`, `atomic_decrement`, `atomic_compare_exchange`
- **Thread pool:** `threadpool_create`, `threadpool_execute`, `threadpool_join_all`
- **Barriers:** `barrier_create`, `barrier_wait`, `barrier_destroy`

**Contoh penggunaan:**
```flast
import { thread_create, thread_join, mutex_create, mutex_lock, mutex_unlock } from "packages/threading/threading";

fn worker() -> void {
    println("Worker thread running");
}

fn main() -> i32 {
    let mutex = mutex_create();
    mutex_lock(mutex);
    
    let thread = thread_create(worker);
    thread_join(thread);
    
    mutex_unlock(mutex);
    return 0;
}
```

## Import Syntax

```flast
// Import specific functions
import { function1, function2 } from "path/to/package";

// Import specific functions with different syntax
import function1, function2 from "path/to/package";

// Import all (wildcard)
import * from "path/to/package";
```

## Package Structure

```
packages/
├── std/
│   └── std.fls          # Standard library
├── system/
│   └── system.fls       # System operations
├── time/
│   └── time.fls         # Date/time operations
└── threading/
    └── threading.fls    # Multi-threading
```

## Notes

- Package tidak otomatis di-include, harus menggunakan import explicit
- Semua function yang di-import harus menggunakan `pub` keyword
- Path import relatif dari file yang menggunakan import
- External C functions didefinisikan dengan `extern "C"` 

# Flast Package Dependencies System

Flast menggunakan sistem impor langsung untuk mengakses fungsi-fungsi dari package yang berbeda. Sistem ini dirancang untuk efisiensi dan kemudahan penggunaan.

## Module-Specific Caching System

Sistem caching Flast menggunakan pendekatan module-specific untuk mencegah file cache yang besar:

### Struktur Cache
```
.build/
├── bin/                    # Executable files
└── cache/
    ├── main_*.o           # Main compilation objects
    └── modules/           # Module-specific cache
        ├── packages/
        │   ├── std/
        │   │   └── std_[hash].cache
        │   ├── system/
        │   │   └── system_[hash].cache
        │   └── time/
        │       └── time_[hash].cache
        └── external_[hash]/
            └── module_[hash].cache
```

### Keuntungan Module Caching:
1. **File Cache Kecil**: Setiap module memiliki cache terpisah
2. **Rebuild Efisien**: Hanya module yang berubah yang di-compile ulang
3. **Paralelisasi**: Multiple modules dapat di-cache secara bersamaan
4. **Organized Structure**: Cache tersusun berdasarkan path module

### Cache Management:
- `flast --clean`: Membersihkan semua cache (main + modules)
- Cache otomatis invalid jika source file lebih baru
- Memory cache untuk akses cepat selama compilation session

## Syntax Import

Untuk mengimpor fungsi dari package, gunakan syntax berikut:

```flast
import { function1, function2 } from "path/to/package";
```

### Contoh:
```flast
import { list_push, list_pop } from "std/std";
import { sleep, exit } from "system/system";
import { now, format_datetime } from "time/time";
import { thread_create, mutex_create } from "threading/threading";
``` 