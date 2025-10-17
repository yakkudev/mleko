#pragma once

#include <common/types.h>


typedef struct {
    u32 id;
    usize size;
    void* contents;
} ShaderAsset;

typedef struct {
    ShaderAsset* shaders;
} Assets;

void asset_manager_init();
void asset_manager_destroy();

void* read_file(const char* name, usize* out_size);

ShaderAsset load_shader_asset(const char* name);
void delete_shader_asset(ShaderAsset asset);

