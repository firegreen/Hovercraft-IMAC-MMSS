CC = gcc
CCFLAGS = -Wall -std=c11 -g
LDFLAGS  = -lm
APP_BIN = hovercraft
SRC_PATH = src
OBJ_PATH = obj
INC_PATH = include
BIN_PATH = bin
LIB_PATH = lib
MKDIR_CMD = mkdir
TEST_DIR = if

SRC_FILES = $(wildcard $(SRC_PATH)/*.c) $(wildcard $(SRC_PATH)/*/*.c)
OBJ_FILES = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(SRC_FILES))

ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    LDFLAGS += -lSDL -lSDL_image
    W_OBJ_FILES = $(subst /,\,$(OBJ_FILES))
    RM_CMD = for %%x in ($(W_OBJ_FILES)) do (if exist %%x (del /q %%x))
    LDFLAGS += -lmingw32 -lSDLmain  -lSDL_image -lopengl32 -lglu32 -lfreeglut
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CCFLAGS += -D AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        CCFLAGS += -D IA32
    endif
else
    MKDIR_CMD+= -p
    RM_CMD = rm $(OBJ_FILES) $(BIN_PATH)/$(APP_BIN)
    ifeq ($(UNAME_S),Linux)
	LDFLAGS += -lGL -lGLU -lglut
	LDFLAGS += -lSDL -lSDL_image
	CCFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -I/Library/Frameworks/SDL.framework/Headers
	LDFLAGS += -I/Library/Frameworks/SDL_image.framework/Headers -I/opt/local/include
	LDFLAGS += `sdl-config --libs` -framework Cocoa -framework OpenGL -lSDL_image -lglut
	CCFLAGS += `sdl-config --cflags`
        CCFLAGS += -D OSX
    endif
endif

all: $(OBJ_PATH) $(BIN_PATH) $(APP_BIN)
	@echo "$(APP_BIN) has been created"
$(OBJ_PATH):
	@echo "------------Make directory $<---------------------"
	if not exist "$(OBJ_PATH)" $(MKDIR_CMD) $(OBJ_PATH)
$(BIN_PATH):
	@echo "------------Make directory $<---------------------"
	if not exist "$(BIN_PATH)" $(MKDIR_CMD) $(BIN_PATH)

$(APP_BIN): $(OBJ_FILES)
	$(CC) -o $(BIN_PATH)/$(APP_BIN) $^ $(CCFLAGS) $(LDFLAGS) -I $(INC_PATH)

$(OBJ_PATH)/main.o: $(SRC_PATH)/main.c
	@echo "------------COMPILATION $< -> $@---------------------"
	$(CC) -c $< -o $@ -I $(INC_PATH)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(INC_PATH)/%.h
	@echo "------------COMPILATION $< -> $@---------------------"
	if not exist "$(@D)" $(MKDIR_CMD) "$(@D)"
	$(CC) -c $< -o $@ -I $(INC_PATH)

clean:
	$(RM_CMD)
