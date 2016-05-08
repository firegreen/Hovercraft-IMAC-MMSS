CC = gcc
CCFLAGS = -Wall -g -O2
#-std=c11
LDFLAGS  = -lm
APP_BIN = hovercraft
SRC_PATH = src
OBJ_PATH = obj
INC_PATH = ./include
BIN_PATH = bin
LIB_PATH = lib

ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    OBJ_PATH =obj/WIN32
    BIN_PATH =bin/WIN32
    LDFLAGS += -lSDL -lmingw32 -lSDLmain  -lSDL_image -lopengl32 -lglu32 -lfreeglut
    IF_N_EXIST = if not exist
    W_OBJ_FILES = $(subst /,\,$(OBJ_FILES))
    CLEAN_CMD = for %%x in ($(W_OBJ_FILES)) do (if exist %%x (del /q %%x))
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
	#CCFLAGS += -D AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
	#CCFLAGS += -D IA32
    endif
else
    IF_N_EXIST = if [ ! -d
    THEN = ]; then
    ENDIF =; fi
    ifeq ($(shell uname),Darwin)
        CCFLAGS += -I/opt/local/include
        CCFLAGS += -I/Library/Frameworks/SDL.framework/Headers -I/Library/Frameworks/SDL_image.framework/Headers
        LDFLAGS += `sdl-config --libs` `sdl-config --cflags`
        CCFLAGS += -framework Cocoa -framework OpenGL -lSDL_image -lglut
        CCFLAGS += -D OSX
        OBJ_PATH =obj/OSX
        BIN_PATH =bin/OSX
    else
        LDFLAGS += -lGL -lGLU -lglut -lSDL -lSDL_image
        CCFLAGS += -D LINUX
        OBJ_PATH =obj/UNIX
        BIN_PATH =bin/UNIX
    endif
    CLEAN_CMD = rm $(OBJ_FILES) $(BIN_PATH)/$(APP_BIN)
endif
SRC_FILES = $(wildcard $(SRC_PATH)/*.c) $(wildcard $(SRC_PATH)/*/*.c)
OBJ_FILES = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(SRC_FILES))

all: $(OBJ_PATH) $(BIN_PATH) $(APP_BIN)
	@echo "$(APP_BIN) has been created"

$(OBJ_PATH):
	@echo "------------Make directory $<---------------------"
	$(IF_N_EXIST) "$(OBJ_PATH)" $(THEN) mkdir "$(OBJ_PATH)" $(ENDIF)

$(BIN_PATH):
	@echo "------------Make directory $<---------------------"
	$(IF_N_EXIST) "$(BIN_PATH)" $(THEN) mkdir "$(BIN_PATH)" $(ENDIF)

$(APP_BIN): $(OBJ_FILES)
	$(CC) -o $(BIN_PATH)/$(APP_BIN) $^ $(CCFLAGS) $(LDFLAGS) -I $(INC_PATH)

$(OBJ_PATH)/main.o: $(SRC_PATH)/main.c
	@echo "------------COMPILATION $< -> $@---------------------"
	$(CC) -c $< -o $@ -I $(INC_PATH) $(CCFLAGS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(INC_PATH)/%.h
	@echo "------------COMPILATION $< -> $@---------------------"
	$(IF_N_EXIST) "$(@D)" $(THEN) mkdir "$(@D)" $(ENDIF)
	$(CC) -c $< -o $@ -I $(INC_PATH) $(CCFLAGS)
clean:
	$(CLEAN_CMD)
