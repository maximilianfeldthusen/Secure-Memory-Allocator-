


/*=====================================================================
 * secure_mem.h – public API
 *====================================================================*/
/*
 * Secure Memory Allocator – Thread‑safe, Zero‑On‑Free
 *
 * Copyright (c) 2026 <Maximilian Feldthusen>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * SPDX‑License‑Identifier: GPL-3.0-or-later
 */


#ifndef SECURE_ALLOC_H
#define SECURE_ALLOC_H

#include <stddef.h>
#include <stdbool.h>

/* Opaque handle for the allocator – a singleton in this example */
typedef struct SecureAllocator SecureAllocator;

/* Initialise the global allocator. Must be called once before any other
   function. Returns true on success. */
bool sa_init(void);

/* Release all internal resources. After this call no other allocator
   function may be used unless sa_init() is called again. */
void sa_shutdown(void);

/* Allocate `size` bytes. The returned pointer is page‑aligned, zeroed,
   and will be cleared on free. Returns NULL on failure. */
void *sa_malloc(size_t size);

/* Reallocate a previously allocated block. Behaviour mirrors realloc()
   but guarantees zero‑padding of any newly‑added bytes. */
void *sa_realloc(void *ptr, size_t new_size);

/* Securely free a block. The memory is overwritten with zeros before
   being released back to the OS. */
void sa_free(void *ptr);

#endif /* SECURE_ALLOC_H */



