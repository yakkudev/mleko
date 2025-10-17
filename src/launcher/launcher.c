#define LOG_PREFIX "<launcher>"

#include <common/log.h>
#include <common/game_api.h>
#include <common/types.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include <dlfcn.h>
#include <sys/inotify.h>
#include <unistd.h>

static const char* GAMELIB_PATH = "./libgame.so";
static const char* GAMELIB_TOUCHPOINT = "./touch.point";

static void* game_lib = NULL;
static GameAPI game = {0};
static GLFWwindow* window = NULL;

static f64 last_reload = -1.0;

void close_game(void) {
    if (!game_lib)
        return;

    LOG("closing libgame");

    if(game.shutdown) game.shutdown();

    dlclose(game_lib);
    game_lib = NULL;
}

void load_game(void) {
    // reload timeout
    const f64 load_start = glfwGetTime();

    if (load_start - last_reload < 1.0)
        return;

    if (game_lib)
        close_game(); 


    LOG("loading libgame");
    game_lib = dlopen(GAMELIB_PATH, RTLD_NOW);
    if (!game_lib) {
        ERROR("dlopen failed: %s", dlerror());
        return;
    }

    GameAPI* (*get_api)() = dlsym(game_lib, "get_game_api");
    if (!get_api) {
        ERROR("dlsym failed: %s", dlerror());
        dlclose(game_lib);
        game_lib = NULL;
        return;
    }

    game = *get_api();
    if (game.init) game.init(window);
    const f64 load_end = glfwGetTime();
    LOG("finished loading libgame in %f seconds", load_end - load_start);
    last_reload = load_end;
}

int main(void) {
    if (!glfwInit()) {
        ERROR("glfw init failed");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "Launcher", NULL, NULL);
    if (!window) {
        ERROR("couldn't create window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    load_game();

#ifndef _WIN32
    i32 fd = inotify_init1(IN_NONBLOCK);
    i32 wd = inotify_add_watch(fd, GAMELIB_TOUCHPOINT, IN_CLOSE_WRITE);
#endif

    f64 last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        f64 now = glfwGetTime();
        f32 dt = (f32)(now - last_time);
        last_time = now;

#ifndef _WIN32
        struct inotify_event ev;
        if (read(fd, &ev, sizeof(ev)) > 0)
            load_game();
#endif

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            load_game();

        if (game.update) game.update(dt);
        if (game.render) game.render();

        glfwSwapBuffers(window);
    }

    close_game();

#ifndef _WIN32
    inotify_rm_watch(fd, wd);
#endif

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
