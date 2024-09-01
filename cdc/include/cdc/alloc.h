
#ifndef DK_ALLOC_H
#define DK_ALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef void *(*dk_alloc_fn_t)(void *state, size_t size);
typedef void (*dk_free_fn_t)(void *state, void *ptr);

typedef struct {
    dk_alloc_fn_t alloc;
    dk_free_fn_t free;
} dk_alloc_t;

typedef struct {
    dk_alloc_t vtable;    
} dk_malloc_t;

typedef struct {
    dk_alloc_t vtable;
    uint8_t *buf;
    size_t ptr;
    size_t cap;
} dk_arena_t;

void *dk_alloc(dk_alloc_t *state, size_t size) {
    return state->alloc(state, size);
}

void dk_free(dk_alloc_t *state, void *ptr) {
    state->free(state, ptr);
}

void *dk_libc_alloc(void *s, size_t size) {
    (void)s;
    return malloc(size);
}

void dk_libc_free(void *s, void *ptr) {
    (void)s;
    free(ptr);
}

dk_malloc_t dk_malloc = {
    .vtable = {
        .alloc = dk_libc_alloc,
        .free = dk_libc_free
    }
};

#endif
