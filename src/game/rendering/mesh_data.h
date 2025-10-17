#pragma once

#include <common/backend.h>
#include <common/types.h>
#include "vertex.h"

typedef struct {
    Vertex* vertices;
    u32 vertex_count;
    u32* indices;
    u32 index_count;
} MeshData;

