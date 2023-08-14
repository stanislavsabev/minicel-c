CC = gcc
STD = -std=c17
CFLAGS = -Wall -Wextra -g $(STD)
DFLAGS = DEBUG

# Directories
SRC_DIR = ./src
INC_DIR = ./include
LIB_DIR = ./lib
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Libraries
LIBFXC_SRC_DIR = $(PROJECTS_DIR)/fx-c
LIBFXC_INC_DIR = $(INC_DIR)/fx
LIBFXC = libfxc.a

# Target name
TARGET = main

SRCS=$(wildcard $(SRC_DIR)/*.c)
HSRCS=$(wildcard $(SRC_DIR)/*.h)
OBJS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DFLAGS := $(addprefix -D,$(DFLAGS))

all: build

help: ## Show this message
	@awk 'BEGIN {FS = ":.*##"; printf "Usage:\n  make \033[36m<target>\033[0m\n\nTargets:\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 }' $(MAKEFILE_LIST)

makedirs: ## Create build directories
	@mkdir -p $(INC_DIR) $(LIB_DIR) $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(TARGET): $(OBJS) $(HSRCS)
	$(CC) $(CFLAGS) $(DFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET) -I$(INC_DIR) -L$(LIB_DIR) -l:$(LIBFXC)

build: makedirs source_libs Makefile $(TARGET) ## Build target

rebuild: clean build ## Clean and rebuild target

run: $(TARGET) ## Run target
	./$(BIN_DIR)/$(TARGET)

release: CFLAGS=-Wall $(STD) -O2 -DNDEBUG
release: clean
release: $(TARGET) ## Build release

check: ## Run valgrind memory sanitizer
	@valgrind --undef-value-errors=no  $(BIN_DIR)/$(TARGET)

clean: ## Clean up build directories
	$(RM) $(OBJ_DIR)/* $(BIN_DIR)/* $(LIB_DIR)/* LIBFXC_INC_DIR/*

source_libs: ## Source external libraries
	mkdir -p  $(LIBFXC_INC_DIR)
	@cp -fr $(LIBFXC_SRC_DIR)/include/* $(LIBFXC_INC_DIR)
	@cp -fr $(LIBFXC_SRC_DIR)/lib/* $(LIB_DIR)

.PHONY: help makedirs clean run check build rebuild release source_libs
