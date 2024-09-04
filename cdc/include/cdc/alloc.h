#ifndef DK_ALLOC_H
#define DK_ALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdalign.h>

typedef void* (*dk_alloc_fn_t)(void* state, size_t size);
typedef void (*dk_free_fn_t)(void* state, void* ptr);

typedef struct {
	dk_alloc_fn_t alloc;
	dk_free_fn_t free;
} dk_alloc_t;

typedef struct {
	dk_alloc_t vtable;
} dk_malloc_t;

void* dk_alloc(dk_alloc_t* state, size_t size) {
	if (state->alloc) {
		return state->alloc(state, size);
	}

	return NULL;
}

void dk_free(dk_alloc_t* state, void* ptr) {
	if (state->free) {
		state->free(state, ptr);
	}
}

void* dk_realloc(dk_alloc_t* state, void* ptr, size_t from, size_t to) {
	void* new_ptr = dk_alloc(state, to);

	memcpy(new_ptr, ptr, from);
	dk_free(state, ptr);

	return new_ptr;
}

void* dk_libc_alloc(void* s, size_t size) {
	(void) s;
	return malloc(size);
}

void dk_libc_free(void* s, void* ptr) {
	(void) s;
	free(ptr);
}

dk_malloc_t dk_malloc = {
	.vtable = {.alloc = dk_libc_alloc, .free = dk_libc_free}
};

static size_t size_align(size_t size) {
	size_t align = alignof(max_align_t);

	size_t real_size = (size / align) * align;
	if (size % align != 0) {
		real_size += align;
	}

	return real_size;
}

typedef struct {
	dk_alloc_t vtable;
	dk_alloc_t* parent;
	size_t size, ptr;
	uint8_t data[];
} dk_arena_t;

static void* dk_arena_alloc(void* a, size_t size) {
	dk_arena_t* arena = (dk_arena_t*) a;

	size_t real_size = size_align(size);

	if (arena->ptr + real_size >= arena->size) {
		return NULL;
	}

	void* res = arena->data + arena->ptr;
	arena->ptr += real_size;

	return res;
}

dk_alloc_t* dk_arena(dk_alloc_t* parent, size_t size) {
	size_t real_size = size_align(size);

	dk_arena_t* arena = dk_alloc(parent, sizeof(dk_arena_t) + real_size);

	arena->vtable.alloc = dk_arena_alloc;
	arena->vtable.free = NULL;
	arena->parent = parent;
	arena->size = real_size;
	arena->ptr = 0;

	return (dk_alloc_t*) arena;
}

void dk_arena_free(dk_alloc_t* a) {
	dk_arena_t* arena = (dk_arena_t*) a;

	dk_free(arena->parent, arena);
}

#endif
