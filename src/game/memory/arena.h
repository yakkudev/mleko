#pragma once

#include <common/types.h>

typedef struct Arena {
    usize capacity;
    usize position;
    void* data;
} Arena;

Arena arena_create(usize capacity);
void* arena_alloc(Arena* arena, usize size);
void arena_reset(Arena* arena);
void arena_release(Arena* arena);

