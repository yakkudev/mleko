#pragma once

#include "mesh_data.h"
#include <common/backend.h>

typedef struct {
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    MeshData *mesh;
} RenderObject;

RenderObject create_render_object(MeshData *mesh);
void delete_render_object(RenderObject render_obj);
void render_render_object(RenderObject render_obj);

