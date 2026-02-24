## Secure-Memory-Allocator-

Secure Alloc -- A Thread‑Safe Secure Memory Allocator

**License:** GPL‑v3 **Author:** Maximilian Feldthusen **Version:**
0.1.0beta

Overview

`secure_alloc` provides a tiny, **portable**, **thread‑safe** memory
allocator designed for security‑critical applications (e.g.,
cryptographic libraries, password managers). It offers:

- Page‑aligned allocations with a **guard page** to catch overruns.
- Automatic **zero‑fill on allocation** and **secure wipe on free**.
- Simple C11 API (`sa_malloc`, `sa_realloc`, `sa_free`).
- Minimal dependencies -- only the standard C library and OS APIs.

Getting Started

Prerequisites

  -----------------------------------------------------------------------
  Platform                    Required tools
  --------------------------- -------------------------------------------
  Linux/macOS                 `gcc`/`clang`, `make`, `git`

  Windows                     MinGW/MSYS (`gcc`, `make`) **or** Visual
                              Studio command prompt
  -----------------------------------------------------------------------

Using the allocator in your own code Include the header #include
"secure_alloc.h" If you installed the library system‑wide, the header
will be found in /usr/local/include. Otherwise, add the relative path:
#include "src/secure_alloc.h" Typical workflow #include "secure_alloc.h"
#include \<stdio.h\>

int main(void) { if (!sa_init()) { fprintf(stderr, "Failed to initialise
secure allocator`\n`{=tex}"); return 1; }

    /* Allocate a secret buffer */
    size_t secret_len = 256;
    unsigned char *secret = sa_malloc(secret_len);
    if (!secret) {
        fprintf(stderr, "Allocation failed\n");
        sa_shutdown();
        return 1;
    }

    /* Use the buffer – e.g. fill with random data */
    for (size_t i = 0; i < secret_len; ++i) {
        secret[i] = (unsigned char)rand();   // replace with a proper RNG
    }

    /* When done, securely erase and free */
    sa_free(secret);

    /* Clean up global resources */
    sa_shutdown();
    return 0;

} Key points Function Purpose sa_init() Sets up the global
mutex/critical‑section and discovers the OS page size. Must be called
once before any other API call. sa_malloc(size) Returns a page‑aligned,
zero‑filled pointer. The underlying mapping includes a guard page to
trap overruns. sa_realloc(ptr, newsize) Resizes an existing allocation
while preserving data and guaranteeing zero‑padding of any newly added
bytes. sa_free(ptr) Overwrites the entire region with zeros (using a
volatile write loop) and releases the mapping, including the guard page.
sa_shutdown() Destroys the global lock and frees any remaining internal
state. After this call you must not use the allocator unless you call
sa_init() again.
