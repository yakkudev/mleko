#pragma once

#include <common/types.h>

#include "../memory/arena.h"
#include "../rendering/material.h"


typedef struct {
    u32 id;
    usize size;
    void* contents;
} ShaderAsset;

void resman_init();
void resman_destroy();

Material* resman_new_material(Material* material);

void* read_file(const char* name, usize* out_size);
void free_file(void* file_ptr);

ShaderAsset load_shader_asset(const char* name);
void delete_shader_asset(ShaderAsset asset);

