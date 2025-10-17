# compiler
CC       := clang
CFLAGS   := -xc -std=c23 -Wall -Iinclude -Iinclude/thirdparty -Iinclude/thirdparty/cglm/include
LDFLAGS  := -lGL -lglfw -lm

# directories
BIN_DIR	    	:= bin
SRC_DIR      	:= src
THIRDPARTY_DIR	:= $(SRC_DIR)/thirdparty
GAME_DIR     	:= $(SRC_DIR)/game
LAUNCHER_DIR 	:= $(SRC_DIR)/launcher

# targets
GAME_LIB     := $(BIN_DIR)/libgame.so
LAUNCHER_BIN := $(BIN_DIR)/launcher
TOUCH_POINT  := $(BIN_DIR)/touch.point

# source files
THIRDPARTY_SRC  := $(THIRDPARTY_DIR)/gl.c
GAME_SRC     	:= $(GAME_DIR)/game.c $(GAME_DIR)/mesh_data.c $(GAME_DIR)/render_object.c $(GAME_DIR)/shader.c
LAUNCHER_SRC 	:= $(LAUNCHER_DIR)/launcher.c

# default target
all: $(GAME_LIB) $(LAUNCHER_BIN)

# default target
game: $(GAME_LIB)

# game shared lib
$(GAME_LIB): $(THIRDPARTY_SRC) $(GAME_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -shared -fPIC $(THIRDPARTY_SRC) $(GAME_SRC) $(LDFLAGS) -o $@
	touch $(TOUCH_POINT)

# launcher exec
$(LAUNCHER_BIN): $(GAME_LIB) $(THIRDPARTY_SRC) $(LAUNCHER_SRC)
	$(CC) $(CFLAGS) $(THIRDPARTY_SRC) $(LAUNCHER_SRC) $(LDFLAGS) -o $@

# ensure bin dir exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# clean up build outputs
clean:
	rm -f $(GAME_LIB) $(LAUNCHER_BIN) $(TOUCH_POINT)

.PHONY: all clean
