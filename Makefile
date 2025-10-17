# compiler
CC       := clang
CFLAGS   := -xc -std=c23 -Wall -Iinclude -Iinclude/thirdparty -Iinclude/thirdparty/cglm/include
PIC_FLAGS:= -fPIC
LDFLAGS  := -lGL -lglfw -lm

# directories
BIN_DIR          := bin
OBJ_DIR          := $(BIN_DIR)/obj
SRC_DIR          := src
THIRDPARTY_DIR   := $(SRC_DIR)/thirdparty
GAME_DIR         := $(SRC_DIR)/game
LAUNCHER_DIR     := $(SRC_DIR)/launcher

# targets
GAME_LIB         := $(BIN_DIR)/libgame.so
LAUNCHER_BIN     := $(BIN_DIR)/launcher
TOUCH_POINT      := $(BIN_DIR)/touch.point

# source files
THIRDPARTY_SRC   := $(THIRDPARTY_DIR)/gl.c
GAME_SRC         := $(shell find $(GAME_DIR) -type f -name '*.c')
LAUNCHER_SRC     := $(LAUNCHER_DIR)/launcher.c

# object files (mirror structure)
GAME_OBJ         := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(GAME_SRC))
THIRDPARTY_OBJ   := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(THIRDPARTY_SRC))
LAUNCHER_OBJ     := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(LAUNCHER_SRC))

# dependency files
DEPS             := $(GAME_OBJ:.o=.d) $(THIRDPARTY_OBJ:.o=.d) $(LAUNCHER_OBJ:.o=.d)

# default target
all: $(GAME_LIB) $(LAUNCHER_BIN)

# game shared lib
$(GAME_LIB): $(THIRDPARTY_OBJ) $(GAME_OBJ) | $(BIN_DIR)
	$(CC) -shared $(PIC_FLAGS) $^ $(LDFLAGS) -o $@
	touch $(TOUCH_POINT)

# launcher exec
$(LAUNCHER_BIN): $(THIRDPARTY_OBJ) $(LAUNCHER_OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

# object build rule with auto-dependency generation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(PIC_FLAGS) -MMD -MP -c $< -o $@

# ensure bin dir exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# clean up build outputs
clean:
	rm -rf $(BIN_DIR)

# include dependency files if they exist
-include $(DEPS)

.PHONY: all clean
