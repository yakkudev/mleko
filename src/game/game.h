#pragma once

struct GLFWwindow;
void game_init(struct GLFWwindow* window);
void game_update(float dt);
void game_render(void);
void game_shutdown(void);

