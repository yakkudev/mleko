#pragma once

#include "shader.h"
#include <cglm/cglm.h>

#include <common/types.h>

typedef enum {
    UNIFORM_INT,
    UNIFORM_FLOAT,
    UNIFORM_VEC2,
    UNIFORM_VEC3,
    UNIFORM_VEC4,
    UNIFORM_MAT3,
    UNIFORM_MAT4,
} UniformType;

typedef union UniformData {
    union UniformData* ptr;
    i32 i;
    f32 f;
    vec2 v2;
    vec3 v3;
    vec4 v4;
    mat3 m3;
    mat4 m4;
} UniformData;

typedef struct {
    i32 location;
    bool is_ptr;
    UniformType type;
    UniformData data;
} ProgramUniform;

typedef struct {
    const char* name;
    ProgramHandle program;

    u32 uniform_count;
    ProgramUniform* uniforms;
} Material;

void use_material(Material* material);
void destroy_material(Material* material);
