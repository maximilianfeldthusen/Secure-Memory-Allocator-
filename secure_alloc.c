

/*=====================================================================
 * secure_mem.c – implementation
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
 * SPDX‑License‑Identifier: GPL-3.0-or-later
 */


#include "secure_alloc.h"

#if defined(_WIN32) || defined(_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/mman.h>
    #include <pthread.h>
    #include <errno.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*--------------------------------------------------------------------
 * Internal data structures
 *--------------------------------------------------------------------*/

struct SecureAllocator {
#if defined(_WIN32) || defined(_WIN64)
    CRITICAL_SECTION lock;
#else
    pthread_mutex_t lock;
#endif
    size_t page_size;
};

static SecureAllocator g_sa = {0};

/*--------------------------------------------------------------------
 * Helper: platform‑specific page allocation / deallocation
 *--------------------------------------------------------------------*/

static void *platform_alloc_pages(size_t size) {
#if defined(_WIN32) || defined(_WIN64)
    return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    void *p = mmap(NULL, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (p == MAP_FAILED) ? NULL : p;
#endif
}

static bool platform_free_pages(void *addr, size_t size) {
#if defined(_WIN32) || defined(_WIN64)
    return VirtualFree(addr, 0, MEM_RELEASE) != 0;
#else
    return munmap(addr, size) == 0;
#endif
}

/*--------------------------------------------------------------------
 * Helper: zero memory securely (avoids compiler optimisations)
 *--------------------------------------------------------------------*/

static void secure_memzero(void *ptr, size_t len) {
    volatile unsigned char *p = (volatile unsigned char *)ptr;
    while (len--) {
        *p++ = 0;
    }
}

/*--------------------------------------------------------------------
 * Initialisation / shutdown
 *--------------------------------------------------------------------*/

bool sa_init(void) {
#if defined(_WIN32) || defined(_WIN64)
    InitializeCriticalSection(&g_sa.lock);
#else
    if (pthread_mutex_init(&g_sa.lock, NULL) != 0) {
        return false;
    }
#endif

    /* Determine system page size */
#if defined(_WIN32) || defined(_WIN64)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    g_sa.page_size = si.dwPageSize;
#else
    long ps = sysconf(_SC_PAGESIZE);
    if (ps <= 0) return false;
    g_sa.page_size = (size_t)ps;
#endif
    return true;
}

void sa_shutdown(void) {
#if defined(_WIN32) || defined(_WIN64)
    DeleteCriticalSection(&g_sa.lock);
#else
    pthread_mutex_destroy(&g_sa.lock);
#endif
}

/*--------------------------------------------------------------------
 * Core allocation logic
 *--------------------------------------------------------------------*/

static size_t round_up_to_page(size_t n) {
    size_t pg = g_sa.page_size;
    return ((n + pg - 1) / pg) * pg;
}

/* Allocate a block that is at least `size` bytes, rounded up to a page.
   We also allocate a *guard page* after the usable region – it is set
   to PROT_NONE (or PAGE_NOACCESS) so accidental overruns segfault. */
static void *internal_alloc(size_t size) {
    size_t usable = round_up_to_page(size);
    size_t total  = usable + g_sa.page_size;   /* extra guard page */

    void *raw = platform_alloc_pages(total);
    if (!raw) return NULL;

#if defined(_WIN32) || defined(_WIN64)
    DWORD oldProt;
    VirtualProtect((char*)raw + usable, g_sa.page_size,
                   PAGE_NOACCESS, &oldProt);
#else
    mprotect((char*)raw + usable, g_sa.page_size, PROT_NONE);
#endif

    /* Zero the usable part (already zeroed by OS, but be explicit) */
    memset(raw, 0, usable);
    return raw;
}

/* Find the start address of the allocation given a user pointer.
   Because we always return the beginning of the usable region,
   the pointer itself is the base address. */
static void *internal_base(void *ptr) {
    return ptr;
}

/* Total size of the mapping (usable + guard) */
static size_t internal_total_size(void *base) {
    /* In this minimal implementation we store the size in the first
       word of the allocation (hidden from the user). */
    size_t *size_ptr = (size_t *)base;
    return *size_ptr;
}

/*--------------------------------------------------------------------
 * Public API
 *--------------------------------------------------------------------*/

void *sa_malloc(size_t size) {
    if (size == 0) return NULL;

    /* Allocate extra space to store the allocation size at the front */
    size_t request = size + sizeof(size_t);
    void *raw = NULL;

#if defined(_WIN32) || defined(_WIN64)
    EnterCriticalSection(&g_sa.lock);
#else
    pthread_mutex_lock(&g_sa.lock);
#endif

    raw = internal_alloc(request);
    if (raw) {
        /* Store total usable size (including the hidden header) */
        size_t *hdr = (size_t *)raw;
        *hdr = round_up_to_page(request);
        raw = (char *)raw + sizeof(size_t);   /* user visible pointer */
    }

#if defined(_WIN32) || defined(_WIN64)
    LeaveCriticalSection(&g_sa.lock);
#else
    pthread_mutex_unlock(&g_sa.lock);
#endif

    return raw;
}

void *sa_realloc(void *ptr, size_t new_size) {
    if (!ptr) return sa_malloc(new_size);
    if (new_size == 0) {
        sa_free(ptr);
        return NULL;
    }

    /* Retrieve the original allocation base */
    void *base = (char *)ptr - sizeof(size_t);
    size_t old_total = *((size_t *)base);
    size_t old_user  = old_total - sizeof(size_t);

    /* If the new size still fits into the existing page‑rounded block,
       just zero‑pad the tail and return the same pointer. */
    size_t needed = new_size + sizeof(size_t);
    size_t new_total = round_up_to_page(needed);

    if (new_total <= old_total) {
        if (new_size > old_user) {
            memset((char *)ptr + old_user, 0, new_size - old_user);
        }
        return ptr;
    }

    /* Otherwise allocate a fresh block and copy the data */
    void *new_ptr = sa_malloc(new_size);
    if (!new_ptr) return NULL;

    memcpy(new_ptr, ptr, old_user);
    sa_free(ptr);
    return new_ptr;
}

void sa_free(void *ptr) {
    if (!ptr) return;

#if defined(_WIN32) || defined(_WIN64)
    EnterCriticalSection(&g_sa.lock);
#else
    pthread_mutex_lock(&g_sa.lock);
#endif

    void *base = (char *)ptr - sizeof(size_t);
    size_t total = *((size_t *)base);

    /* Overwrite the whole usable region with zeros */
    secure_memzero(base, total);

    /* Release the whole mapping (usable + guard) */
    platform_free_pages(base, total + g_sa.page_size);

#if defined(_WIN32) || defined(_WIN64)
    LeaveCriticalSection(&g_sa.lock);
#else
    pthread_mutex_unlock(&g_sa.lock);
#endif
}


