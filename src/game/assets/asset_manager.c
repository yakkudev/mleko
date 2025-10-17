#include "asset_manager.h"

#include <common/log.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

void asset_manager_init() { 

}

void asset_manager_destroy() { 

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

ShaderAsset load_shader_asset(const char* name) {
    ShaderAsset asset = { };
    asset.contents = read_file(name, &asset.size);
    return asset;
}

void delete_shader_asset(ShaderAsset asset) {
    free(asset.contents);
}

