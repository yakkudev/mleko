#pragma once

#include "mesh_data.h"
#include "material.h"
#include <common/backend.h>

typedef struct {
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    MeshData* mesh;
    Material* material;
} RenderObject;

RenderObject create_render_object(MeshData *mesh, Material* material);
void delete_render_object(RenderObject render_obj);
void render_render_object(RenderObject render_obj);

