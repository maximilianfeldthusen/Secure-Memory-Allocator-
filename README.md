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


