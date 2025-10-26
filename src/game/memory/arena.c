#include "arena.h"
#include <stdlib.h>

Arena arena_create(usize capacity) {
    Arena arena = {
        .capacity = capacity,
        .position = 0,
        .data = malloc(capacity),
    };
    return arena;
}

void* arena_alloc(Arena* arena, usize size) {
    if (arena->position + size > arena->capacity) {
        arena->position = arena->capacity;
        return NULL;
    }
    void* ptr = (u8*)arena->data + arena->position;
    arena->position += size;
    return ptr;
}

void arena_reset(Arena* arena) {
    arena->position = 0;
}

void arena_release(Arena* arena) {
    free(arena->data);
}

