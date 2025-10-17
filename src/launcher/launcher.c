#define LOG_PREFIX "<launcher>"

#include <common/log.h>
#include <common/game_api.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include <dlfcn.h>
#include <sys/inotify.h>
#include <unistd.h>

static const char *GAMELIB_PATH = "./libgame.so";
static const char *GAMELIB_TOUCHPOINT = "./touch.point";

static void *game_lib = NULL;
static GameAPI game = {0};
static GLFWwindow* window = NULL;

void close_game(void) {
    if (!game_lib)
        return;

    LOG("closing libgame");

    if(game.shutdown) game.shutdown();

    dlclose(game_lib);
    game_lib = NULL;
}

void load_game(void) {
    if (game_lib)
        close_game(); 

    const double load_start = glfwGetTime();

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
    LOG("finished loading libgame in %f seconds", glfwGetTime() - load_start);
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
    int fd = inotify_init1(IN_NONBLOCK);
    int wd = inotify_add_watch(fd, GAMELIB_TOUCHPOINT, IN_CLOSE_WRITE);
#endif

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;

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

    if (game.shutdown) game.shutdown();

#ifndef _WIN32
    inotify_rm_watch(fd, wd);
#endif

    glfwDestroyWindow(window);
    glfwTerminate();

    close_game();

    return 0;
}
