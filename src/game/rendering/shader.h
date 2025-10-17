#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <common/types.h>

typedef struct {
    GLuint id;
} ShaderHandle;

typedef struct {
    GLuint id;
} ProgramHandle;

ShaderHandle create_shader(GLenum shader_type, const char* shader_string);
void delete_shader(ShaderHandle shader);

ProgramHandle create_program(ShaderHandle vertex, ShaderHandle fragment);
void delete_program(ProgramHandle program);
void use_program(ProgramHandle program);
