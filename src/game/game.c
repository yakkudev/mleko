#include <stdint.h>

#include "game.h"
#include "cglm/types.h"
#include "cglm/util.h"
#include "cglm/vec3.h"
#include "resources/resource_manager.h"
#include "rendering/material.h"
#include "rendering/mesh_data.h"
#include "rendering/render_object.h"
#include "rendering/shader.h"

#include <common/backend.h>
#include <common/game_api.h>
#include <common/log.h>
#include <common/types.h>
#include <cglm/cam.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define WORLD_UP (vec3){ 0.0f, 1.0f, 0.0f }
#define WORLD_FORWARD (vec3){ 0.0f, 0.0f, -1.0f }
#define WORLD_RIGHT (vec3){ 1.0f, 0.0f, 0.0f }

#define WORLD_DOWN (vec3){ 0.0f, -1.0f, 0.0f }
#define WORLD_BACK (vec3){ 0.0f, 0.0f, 1.0f }
#define WORLD_LEFT (vec3){ -1.0f, 0.0f, 0.0f }

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
static i32 window_height = 600;
static i32 window_width = 800;
static f32 current_time;

Vertex vertices[] = {
    { .pos = {  0.5f,  0.5f,  0.0f }, .uv = { 1.0f, 1.0f } }, // top right
    { .pos = {  0.5f, -0.5f,  0.0f }, .uv = { 1.0f, 0.0f } }, // bottom right
    { .pos = { -0.5f, -0.5f,  0.0f }, .uv = { 0.0f, 0.0f } }, // bottom left
    { .pos = { -0.5f,  0.5f,  0.0f }, .uv = { 0.0f, 1.0f } }, // top left
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

struct Cam {
    vec3 pos;
    f32 yaw, pitch;
    f32 z_near, z_far;
    f32 fovy;

    struct {
        mat4 view;
        mat4 projection;
        vec3 orientation;
    } computed;
} Cam;

f64 last_mouse_x;
f64 last_mouse_y;

ProgramHandle shader_prog;
RenderObject render_obj;

ShaderAsset vertex_src;
ShaderAsset fragment_src;

ProgramUniform std_uniforms[8];

static u32 texture = 0;

void load_gl(void) {
    i32 version = gladLoadGL(glfwGetProcAddress);
    LOG("loaded GL version %d.%d", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
}

void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
}

void framebuffer_resize_callback(GLFWwindow* window, i32 width, i32 height) {
    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);
}


void calculate_cam_matrices() {
    glm_perspective(Cam.fovy, (f32)window_width / (f32)window_height, Cam.z_near, Cam.z_far, Cam.computed.projection);

    Cam.yaw = fmodf(Cam.yaw, GLM_TAUf);
    f32 pitch_bumper = 0.0001;
    Cam.pitch = glm_clamp(Cam.pitch, -GLM_PI_2f + pitch_bumper, GLM_PI_2f - pitch_bumper);

    vec3 look_dir = { 
        cos(Cam.pitch) * sin(Cam.yaw),
        sin(Cam.pitch),
        -cos(Cam.pitch) * cos(Cam.yaw)
    };
    Cam.computed.orientation[0] = look_dir[0];
    Cam.computed.orientation[1] = 0.0;
    Cam.computed.orientation[2] = look_dir[2];
    glm_normalize(Cam.computed.orientation);

    glm_look(Cam.pos, look_dir, WORLD_UP, Cam.computed.view);
}

void game_init(GLFWwindow* window) {
    WARN("game init!");
    glfw_window = window;

    load_gl();
    resman_init();

    glfwSetFramebufferSizeCallback(glfw_window, framebuffer_resize_callback);
    glfwSetKeyCallback(glfw_window, key_callback);

    // make sure width & height are correct
    glfwGetFramebufferSize(glfw_window, &window_width, &window_height);
    framebuffer_resize_callback(glfw_window, window_width, window_height);

    // raw input
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    Cam = (struct Cam) {
        .fovy = glm_rad(80.0),
        .z_near = 0.01,
        .z_far = 1000.0,
        .pos = { 0.0, 0.0, 2.0 }
    };
    calculate_cam_matrices();

    vertex_src = load_shader_asset("../assets/shader/default.vert");
    fragment_src = load_shader_asset("../assets/shader/default.frag");

    {
        ShaderHandle vert_shader = create_shader(GL_VERTEX_SHADER, vertex_src.contents);
        ShaderHandle frag_shader = create_shader(GL_FRAGMENT_SHADER, fragment_src.contents);

        shader_prog = create_program(vert_shader, frag_shader);

        delete_shader(vert_shader);
        delete_shader(frag_shader);
    }


    // FIXME: this might be undefined behavior lol
    { 
        std_uniforms[0] = (ProgramUniform){ 
            .location = glGetUniformLocation(shader_prog.id, "uTime"),
            .type = UNIFORM_FLOAT,
            .is_ptr = true,
            .data.ptr = &current_time
        };

        std_uniforms[1] = (ProgramUniform){ 
            .location = glGetUniformLocation(shader_prog.id, "uView"),
            .type = UNIFORM_MAT4,
            .is_ptr = true,
            .data.ptr = &Cam.computed.view
        };

        std_uniforms[2] = (ProgramUniform){ 
            .location = glGetUniformLocation(shader_prog.id, "uProj"),
            .type = UNIFORM_MAT4,
            .is_ptr = true,
            .data.ptr = &Cam.computed.projection
        };
    }

    Material* mat = resman_new_material(&(Material){"default", shader_prog, 3, std_uniforms});
    render_obj = create_render_object(&mesh, mat);


    const char* prog_textures[] = { "uTexture" };
    setup_program_textures(shader_prog, 1, prog_textures);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    i32 width, height, num_channels;
    stbi_set_flip_vertically_on_load(true);
    u8 *data = stbi_load("../assets/image/img.png", &width, &height, &num_channels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        ERROR("failed to load texture");
    }
    stbi_image_free(data);

}

void game_update(f32 dt) {
    current_time = glfwGetTime() * 2.0f;

    f32 move_speed = 1.0 * dt;
    f64 sensitivity = glm_rad(0.03);

    // mouse input
    f64 mouse_x, mouse_y;
    glfwGetCursorPos(glfw_window, &mouse_x, &mouse_y);        

    f64 mouse_dx = last_mouse_x - mouse_x;
    f64 mouse_dy = last_mouse_y - mouse_y;
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;

    // wasd move
    vec3 wish_dir = {
        (glfwGetKey(glfw_window, GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(glfw_window, GLFW_KEY_A) == GLFW_PRESS),
        0.0,
        (glfwGetKey(glfw_window, GLFW_KEY_W) == GLFW_PRESS) - (glfwGetKey(glfw_window, GLFW_KEY_S) == GLFW_PRESS),
    };
    glm_vec3_scale(wish_dir, move_speed, wish_dir);

    vec3 right = {};
    glm_vec3_cross(Cam.computed.orientation, WORLD_UP, right);
    glm_vec3_normalize(right);

    vec3 move = {};
    glm_vec3_scale(Cam.computed.orientation, wish_dir[2], move);

    vec3 right_move = {};
    glm_vec3_scale(right, wish_dir[0], right_move);
    glm_vec3_add(move, right_move, move);

    glm_vec3_add(Cam.pos, move, Cam.pos);


    // mouse look
    if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        Cam.pitch += mouse_dy * sensitivity;
        Cam.yaw += -mouse_dx * sensitivity;

        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    calculate_cam_matrices();
}

void game_render(void) {
    // LOG("game render!");
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    render_render_object(render_obj);

    u32 error = glGetError();
    while (error) {
        ERROR("opengl error: %d", error);
        error = glGetError();
    }
}

void game_shutdown(void) {
    WARN("game shutdown!");

    glfwSetFramebufferSizeCallback(glfw_window, NULL);
    glfwSetKeyCallback(glfw_window, NULL);

    delete_render_object(render_obj);
    delete_program(shader_prog);

    delete_shader_asset(vertex_src);
    delete_shader_asset(fragment_src);

    resman_destroy();

    glDeleteTextures(1, &texture);
}


