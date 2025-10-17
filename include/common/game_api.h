#pragma once

struct GLFWwindow;
typedef void (*fn_init)(struct GLFWwindow* window);
typedef void (*fn_update)(float);
typedef void (*fn_render)(void);
typedef void (*fn_shutdown)(void);

typedef struct {
    fn_init init;
    fn_update update;
    fn_render render;
    fn_shutdown shutdown;
} GameAPI;

