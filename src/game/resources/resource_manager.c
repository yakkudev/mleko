#include "resource_manager.h"

#include "../rendering/material.h"

#include <common/log.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MATERIALS 512

typedef struct {
    const char* name;
    void* ptr;
} AssetValue;

struct Assets{
    ShaderAsset* shaders;
    AssetValue materials[MAX_MATERIALS];
    Arena material_arena;
} Assets;

void resman_init() { 
    Assets = (struct Assets){ };
    Assets.material_arena = arena_create(MAX_MATERIALS * sizeof(Material));
}

void resman_destroy() { 
    arena_release(&Assets.material_arena);
}

Material* resman_new_material(Material* material) {
    usize size = sizeof(Material);
    Material* ptr = arena_alloc(&Assets.material_arena, size);
    memcpy(ptr, material, size);
    LOG("creating new material: %s", ptr->name);

    return ptr;
}

void resman_delete_material(Material* material) {
    WARN("TODO: cannot delete material yet, convert arena into pool maybe");
}

void* read_file(const char* filename, usize* out_size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        ERROR("file open error: %s", strerror(errno));
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        ERROR("failed to seek end of file: %s", strerror(errno));
        fclose(file);
        return NULL;
    }

    u32 size = ftell(file);
    if (size < 0) {
        ERROR("failed to get file size: %s", strerror(errno));
        fclose(file);
        return NULL;
    }

    rewind(file);

    // + 1 for null terminator
    void* buffer = malloc((usize)size + 1);
    if (!buffer) {
        ERROR("failed to allocate memory: %s", strerror(errno));
        fclose(file);
        return NULL;
    }

    usize read_bytes = fread(buffer, 1, (usize)size, file);
    if (read_bytes != (usize)size) {
        ERROR("failed to read entire file: %s", strerror(errno));
        free(buffer);
        fclose(file);
        return NULL;
    }

    fclose(file);

    // null-terminate our buffer
    ((char*)buffer)[size] = '\0';

    if (out_size) {
        *out_size = (size_t)size;
    }

    return buffer;
}

void free_file(void* file_ptr) {
    free(file_ptr);
}

ShaderAsset load_shader_asset(const char* name) {
    ShaderAsset asset = { };
    asset.contents = read_file(name, &asset.size);
    return asset;
}

void delete_shader_asset(ShaderAsset asset) {
    free_file(asset.contents);
}

