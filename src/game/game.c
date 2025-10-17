#include <stdint.h>

#include "game.h"
#include "mesh_data.h"
#include "render_object.h"
#include "shader.h"

#include <common/backend.h>
#include <common/game_api.h>
#include <common/log.h>
#include <common/types.h>


GameAPI* get_game_api(void) {
    static GameAPI api = {
        .init = game_init,
        .update = game_update,
        .render = game_render,
        .shutdown = game_shutdown,
    };
    return &api;
}

static GLFWwindow* window = NULL;

const char *vertex_src = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aUV;\n"
    "layout (location = 3) in vec4 aColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragment_src = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.0f, 0.2f, 1.0f);\n"
    "}\n\0";


Vertex vertices[] = {
    { .pos = {  0.5f,  0.5f,  0.0f } }, // top right
    { .pos = {  0.5f, -0.5f,  0.0f } }, // bottom right
    { .pos = { -0.5f, -0.5f,  0.0f } }, // bottom left
    { .pos = { -0.5f,  0.5f,  0.0f } }, // top left
};
u32 indices[] = { 
    0, 1, 3,
    1, 2, 3 
};

MeshData mesh = {
    .vertices = vertices,
    .vertex_count = 4,
    .indices = indices,
    .index_count = 6,
};

ProgramHandle shader_prog;
RenderObject render_obj;

void load_gl(void) {
    i32 version = gladLoadGL(glfwGetProcAddress);
    LOG("loaded GL version %d.%d", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
}


void game_init(GLFWwindow* _window) {
    window = _window;
    WARN("game init!");

    load_gl();

    ShaderHandle vert_shader = create_shader(GL_VERTEX_SHADER, vertex_src);
    ShaderHandle frag_shader = create_shader(GL_FRAGMENT_SHADER, fragment_src);

    shader_prog = create_program(vert_shader, frag_shader);
    render_obj = create_render_object(&mesh);

    delete_shader(vert_shader);
    delete_shader(frag_shader);
}

void game_update(f32 dt) {
    // LOG("game update! delta: %f", dt);
}

void game_render(void) {
    // LOG("game render!");
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    use_program(shader_prog);

    glBindVertexArray(render_obj.VAO);
    glDrawElements(GL_TRIANGLES, render_obj.mesh->index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    u32 error = glGetError();
    while (error) {
        ERROR("opengl error: %d", error);
        error = glGetError();
    }
}

void game_shutdown(void) {
    WARN("game shutdown!");

    delete_render_object(render_obj);
    delete_program(shader_prog);
}


