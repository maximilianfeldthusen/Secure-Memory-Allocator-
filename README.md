

Here's the text converted to GitHub-flavored Markdown:

````markdown
# Secure Alloc -- A Thread‑Safe Secure Memory Allocator

**License:** GPL‑v3  
**Author:** Maximilian Feldthusen  
**Version:** 0.1.0beta

## Overview

`secure_alloc` provides a tiny, **portable**, **thread‑safe** memory allocator designed for security‑critical applications (e.g., cryptographic libraries, password managers). It offers:

- Page‑aligned allocations with a **guard page** to catch overruns.
- Automatic **zero‑fill on allocation** and **secure wipe on free**.
- Simple C11 API (`sa_malloc`, `sa_realloc`, `sa_free`).
- Minimal dependencies -- only the standard C library and OS APIs.

## Getting Started

### Prerequisites

| Platform        | Required tools                                                  |
|-----------------|---------------------------------------------------------------|
| Linux/macOS     | `gcc`/`clang`, `make`, `git`                                   |
| Windows         | MinGW/MSYS (`gcc`, `make`) **or** Visual Studio command prompt |

### Build & Install (POSIX)

```bash
git clone https://github.com/your/repo.git
cd repo
make    # builds libsecurealloc.a
make test    # runs unit tests
sudo make install    # copies lib and header to /usr/local
````

### Build & Install (Windows -- MinGW)

```bash
git clone https://github.com/your/repo.git
cd repo
mingw32-make    # builds securealloc.lib
mingw32-make test    # runs tests
mingw32-make install DESTDIR=C:\Program Files\SecureAlloc
```

## Project Directory Tree

```
secure-alloc/
│
├─ src/                   # Source files for the library
│  ├─ secure_alloc.c
│  └─ secure_alloc.h
│
├─ test/                  # Unit-test harness
│  ├─ tests.c
│  ├─ unity.c             # Unity framework source (downloaded once)
│  └─ unity.h
│
├─ docs/                  # Human-readable documentation
│  ├─ README.md
│  └─ LICENSE             # GPL‑v3 text (or a copy‑link to it)
│
├─ Makefile               # Build script (POSIX + MinGW)
└─ .gitignore             # (optional) ignore object files, binaries, etc.
```

## Explanation of Each Item

| Path                    | What it contains                                                                                                                 | Why it matters                                            |
| ----------------------- | -------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------- |
| `src/secure_alloc.c`    | Implementation of the allocator (platform‑specific page handling, guard pages, secure wipe, mutex/critical‑section).             | Core library code.                                        |
| `src/secure_alloc.h`    | Public API (sa_init, sa_malloc, ...).                                                                                            | Header that client programs include.                      |
| `test/tests.c`          | Small test suite built on the Unity framework. Verifies correctness and security properties.                                     | Ensures the library works correctly.                      |
| `test/unity.{c,h}`      | Single‑header test framework (download from [https://github.com/ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity)). | No external dependencies for testing.                     |
| `docs/README.md`        | Overview, build instructions, usage examples.                                                                                    | First place a newcomer looks.                             |
| `docs/LICENSE`          | GPL‑v3 license text.                                                                                                             | Required for open‑source distribution.                    |
| `Makefile`              | Handles compilation of the static library (libsecurealloc.a or securealloc.lib), the test binary, and install/uninstall targets. | One‑stop build system for both POSIX and Windows (MinGW). |
| `.gitignore` (optional) | Excludes `*.o`, test, `lib*.a`, etc. from version control.                                                                       | Keeps the repository tidy.                                |

## Using the Allocator in Your Own Code

### Include the header

```c
#include "secure_alloc.h"
```

If you installed the library system‑wide, the header will be found in `/usr/local/include`. Otherwise, add the relative path:

```c
#include "src/secure_alloc.h"
```

### Typical Workflow

```c
#include "secure_alloc.h"
#include <stdio.h>

int main(void) {
    if (!sa_init()) {
        fprintf(stderr, "Failed to initialise secure allocator\n");
        return 1;
    }

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
}
```

### Key Points

| Function                   | Purpose                                                                                                                                             |
| -------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| `sa_init()`                | Sets up the global mutex/critical‑section and discovers the OS page size. Must be called once before any other API call.                            |
| `sa_malloc(size)`          | Returns a page‑aligned, zero‑filled pointer. The underlying mapping includes a guard page to trap overruns.                                         |
| `sa_realloc(ptr, newsize)` | Resizes an existing allocation while preserving data and guaranteeing zero‑padding of any newly added bytes.                                        |
| `sa_free(ptr)`             | Overwrites the entire region with zeros (using a volatile write loop) and releases the mapping, including the guard page.                           |
| `sa_shutdown()`            | Destroys the global lock and frees any remaining internal state. After this call, you must not use the allocator unless you call `sa_init()` again. |

## License

GPL‑v3 -- see the [LICENSE](docs/LICENSE) file.

## Contact / Issues

Please open an issue on the [SourceForge tracker](https://sourceforge.net/p/your-project/issues).

```

This markdown format follows GitHub's conventions for headings, code blocks, tables, and list formatting, and ensures readability on GitHub's platform.
```
