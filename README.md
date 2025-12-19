# ft_malloc - Custom Dynamic Memory Allocator

![Language](https://img.shields.io/badge/language-C-blue)
![Topic](https://img.shields.io/badge/topic-Memory%20Management-orange)
![Technique](https://img.shields.io/badge/tech-mmap%20%2F%20munmap-red)
![Bonus](https://img.shields.io/badge/features-Coalescing%20%7C%20Env%20Debug-brightgreen)

<br />
<p align="center">
  <h3 align="center">A high-performance implementation of malloc, free, and realloc</h3>
</p>

## 🗣️ About The Project

**ft_malloc** is a custom implementation of the C standard library's dynamic memory management functions. Instead of using `sbrk` (the legacy method), this allocator interacts directly with the Kernel via **`mmap`** and **`munmap`** system calls to request and release memory pages.

This project tackles the core challenges of systems programming: **fragmentation reduction**, **memory alignment**, and **runtime configuration** via environment variables.

### 🎯 Key Engineering Goals
- **Virtual Memory Management:** Direct interaction with the OS paging system (`sysconf(_SC_PAGESIZE)`).
- **Optimization:** Use of pre-allocated "Zones" (TINY, SMALL, LARGE) to minimize the number of system calls (context switches).
- **Metadata Overhead:** Embedding block headers (`struct`) directly into the heap without corruption.
- **Defragmentation:** Implementing algorithms to merge free blocks and recycle memory efficiently.

---

## 🏗️ Memory Architecture

To optimize performance, the allocator categorizes requests based on size. This strategy reduces internal fragmentation and system call overhead.

### 1. Zone Strategy
* **TINY Zone:** Stores blocks $\le$ 128 bytes. Pre-allocates ~128KB.
* **SMALL Zone:** Stores blocks $\le$ 1024 bytes. Pre-allocates ~1MB.
* **LARGE Zone:** Requests > 1024 bytes. Each allocation is a dedicated `mmap` call.

### 2. Block Structure
Every allocation is preceded by a metadata header hidden from the user, which creates a doubly linked list embedded in the memory itself.

~~~c
typedef struct s_block {
    struct s_block  *next;      // Pointer to next block
    struct s_block  *prev;      // Pointer to previous block (for coalescing)
    size_t          size;       // Data size (excluding header)
    int             free;    // Availability flag
} t_block;
~~~

### 3. Memory Layout Visualization
The heap is managed as a contiguous block of memory where headers and user data are interleaved.

~~~text
 -------------------------------------------------------------------
 |  BLOCK HEADER (32 bytes)   |      USER PAYLOAD (Variable)       |
 -------------------------------------------------------------------
 | prev: 0x7f...00            |                                    |
 | next: 0x7f...90            |   [  Your Data Goes Here...  ]     |
 | size: 64 bytes             |   [  "Hello World", Integers ]     |
 | free: 0 (Allocated)        |                                    |
 -------------------------------------------------------------------
                              ^
                              |
                     Pointer returned to user
~~~

When multiple blocks exist in a zone, they form a chain:

~~~text
[ ZONE START ]
      |
      v
+------------+------------+   +------------+------------+   +------------+
| HEADER (1) |  DATA (1)  |-->| HEADER (2) |   EMPTY    |-->| HEADER (3) | ...
+------------+------------+   +------------+------------+   +------------+
  (Allocated)                   (Free Space)                  (Allocated)
~~~

---

## 🚀 Advanced Features (Bonuses)

### 🧩 1. Defragmentation (Coalescing)
To prevent "External Fragmentation" (where enough total memory exists but is divided into small, unusable chunks), `free()` implements **block coalescing**.
* **Logic:** When a block is freed, the allocator checks its immediate neighbors (`prev` and `next`) using pointer arithmetic.
* **Action:** If neighbors are free, they are merged into a single larger block in $O(1)$ time, updating the linked list pointers accordingly.

### 🛠️ 2. Runtime Debugging (Env Variable)
The library behavior can be modified at runtime without recompilation.
* **Feature:** Detects if the environment variable `MALLOC_DEBUG` is set.
* **Behavior:** When active, the allocator prints detailed logs to `stderr` (allocation size, pointer addresses, zone info), allowing developers to trace memory leaks or unexpected behavior during execution.

### 🔬 3. Advanced Memory Inspection (Hex Dump)
Implemented `show_alloc_mem_ex()` to visualize the exact content of the heap memory in hexadecimal format.
* **Utility:** Critical for debugging buffer overflows or analyzing how data is laid out in memory bytes.

---

## 🛠️ Performance & Alignment

* **16-Byte Alignment:** All pointers returned are aligned to 16 bytes (128 bits) to ensure performance on x86_64 processors and compatibility with SSE/AVX instructions.
* **Page Size Awareness:** Allocation sizes are calculated as multiples of the system page size (`getpagesize()`) to align with the OS virtual memory manager.

---

## 💻 Usage

### Build the Library
~~~bash
make
# Generates libft_malloc_x86_64_Linux.so
~~~

### Inject into existing programs
You can use **LD_PRELOAD** to force existing binaries (like `ls` or `vim`) to use your allocator instead of the system default.

~~~bash
# Run 'ls' using my custom malloc
export LD_LIBRARY_PATH=$PWD
export LD_PRELOAD=libft_malloc_x86_64_Linux.so

ls
~~~

### Link in your C code
~~~c
#include "ft_malloc.h"

int main() {
    char *str = ft_malloc(50);
    ft_strcpy(str, "Hello Custom Heap!");
    
    // Enable debug via Env Variable to see internal logs
    // export MALLOC_DEBUG=1
    
    ft_free(str);
    return 0;
}
~~~

---
*Developed by Eleder Andres. Engineering memory, one bit at a time.*
