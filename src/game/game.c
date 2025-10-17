#include <stdint.h>

#include "game.h"
#include "assets/asset_manager.h"
#include "rendering/mesh_data.h"
#include "rendering/render_object.h"
#include "rendering/shader.h"

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

static GLFWwindow* glfw_window = NULL;

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

ShaderAsset vertex_src;
ShaderAsset fragment_src;

void load_gl(void) {
    i32 version = gladLoadGL(glfwGetProcAddress);
    LOG("loaded GL version %d.%d", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
}


void game_init(GLFWwindow* window) {
    WARN("game init!");
    glfw_window = window;

    glfwSetKeyCallback(glfw_window, key_callback);

    load_gl();
    vertex_src = load_shader_asset("../assets/shader/default.vert");
    fragment_src = load_shader_asset("../assets/shader/default.frag");

    ShaderHandle vert_shader = create_shader(GL_VERTEX_SHADER, vertex_src.contents);
    ShaderHandle frag_shader = create_shader(GL_FRAGMENT_SHADER, fragment_src.contents);

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

    glfwSetKeyCallback(glfw_window, NULL);

    delete_render_object(render_obj);
    delete_program(shader_prog);

    delete_shader_asset(vertex_src);
    delete_shader_asset(fragment_src);
}


