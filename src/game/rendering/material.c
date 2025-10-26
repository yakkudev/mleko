#include "material.h"
#include "shader.h"

#include <common/backend.h>
#include <memory.h>

void set_uniform(ProgramUniform* uniform) {
    UniformData* data = &uniform->data;
    if (uniform->is_ptr && uniform->data.ptr != NULL) {
        data = uniform->data.ptr;
    }
    switch (uniform->type) {
    case UNIFORM_INT:
        glUniform1i(uniform->location, data->i);
        break;
    case UNIFORM_FLOAT:
        glUniform1f(uniform->location, data->f);
        break;
    case UNIFORM_VEC2:
        glUniform2f(
            uniform->location,
            data->v2[0],
            data->v2[1]
        );
        break;
    case UNIFORM_VEC3:
        glUniform3f(
            uniform->location, 
            data->v3[0],
            data->v3[1],
            data->v3[2]
        );
        break;
    case UNIFORM_VEC4:
        glUniform4f(
            uniform->location,
            data->v4[0],
            data->v4[1],
            data->v4[2],
            data->v4[3]
        );
        break;
    case UNIFORM_MAT3:
        glUniformMatrix3fv(
            uniform->location,
            1,
            GL_FALSE,
            (float*)data->m3
        );
        break;
    case UNIFORM_MAT4:
        glUniformMatrix4fv(
            uniform->location,
            1,
            GL_FALSE,
            (float*)data->m4
        );
        break;
    }
}

void use_material(Material* material) {
    use_program(material->program);
    for (u32 i = 0; i < material->uniform_count; i++) {
        set_uniform(&material->uniforms[i]);
    }
}
