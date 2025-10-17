#include <common/log.h>

#include "shader.h"

#define LOG_SIZE 512
static char INFO_LOG[LOG_SIZE];

ShaderHandle create_shader(GLenum shader_type, const char* shader_string) {
    ShaderHandle shader = { .id = glCreateShader(shader_type) };
    glShaderSource(shader.id, 1, &shader_string, NULL);
    glCompileShader(shader.id);

    // compile errors 
    i32 success;
    glGetShaderiv(shader.id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader.id, LOG_SIZE, NULL, INFO_LOG);
        ERROR("shader compilation failed: %s", INFO_LOG);
    }

    return shader;
}

ProgramHandle create_program(ShaderHandle vertex, ShaderHandle fragment) {
    ProgramHandle program = { .id = glCreateProgram() };
    glAttachShader(program.id, vertex.id);
    glAttachShader(program.id, fragment.id);
    glLinkProgram(program.id);

    // link errors
    i32 success;
    glGetProgramiv(program.id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program.id, 512, NULL, INFO_LOG);
        ERROR("shader linking failed: %s", INFO_LOG);
    }

    LOG("created shader program with handle %d (vert %d, frag %d)", program.id, vertex.id, fragment.id);
    return program;
}

void delete_shader(ShaderHandle shader) {
    glDeleteShader(shader.id);
}

void delete_program(ProgramHandle program) {
    glDeleteProgram(program.id);
}

void use_program(ProgramHandle program) {
    glUseProgram(program.id);
}

