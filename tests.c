


/*=====================================================================
 * test_secure_mem.c – unit tests for secure_mem
 *====================================================================*/
/*
 * Secure Memory Allocator – Thread‑safe, Zero‑On‑Free
 *
 * Copyright (c) 2026 <Maximilian Feldthusen>
 *
 * SPDX‑License‑Identifier: GPL-3.0-or-later
 */


#include "unity.h"
#include "secure_alloc.h"
#include <string.h>

void setUp(void)    { /* called before each test */ }
void tearDown(void) { /* called after each test */ }

void test_basic_allocation(void) {
    const size_t N = 1024;
    void *p = sa_malloc(N);
    TEST_ASSERT_NOT_NULL(p);
    /* Memory must be zeroed */
    for (size_t i = 0; i < N; ++i) {
        TEST_ASSERT_EQUAL_UINT8(0, ((unsigned char*)p)[i]);
    }
    sa_free(p);
}

void test_realloc_grow(void) {
    void *p = sa_malloc(64);
    TEST_ASSERT_NOT_NULL(p);
    memset(p, 0xAA, 64);
    p = sa_realloc(p, 200);
    TEST_ASSERT_NOT_NULL(p);
    /* Old data must survive */
    for (size_t i = 0; i < 64; ++i) {
        TEST_ASSERT_EQUAL_UINT8(0xAA, ((unsigned char*)p)[i]);
    }
    /* New tail must be zeroed */
    for (size_t i = 64; i < 200; ++i) {
        TEST_ASSERT_EQUAL_UINT8(0, ((unsigned char*)p)[i]);
    }
    sa_free(p);
}

void test_free_wipes_memory(void) {
    const size_t N = 128;
    unsigned char *p = sa_malloc(N);
    TEST_ASSERT_NOT_NULL(p);
    memset(p, 0xFF, N);
    sa_free(p);

    /* Re‑allocate the same size – we should get a fresh zeroed block */
    unsigned char *q = sa_malloc(N);
    TEST_ASSERT_NOT_NULL(q);
    for (size_t i = 0; i < N; ++i) {
        TEST_ASSERT_EQUAL_UINT8(0, q[i]);
    }
    sa_free(q);
}

/* ----------------------------------------------------------------- */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_basic_allocation);
    RUN_TEST(test_realloc_grow);
    RUN_TEST(test_free_wipes_memory);
    return UNITY_END();
}



