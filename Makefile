# Compiler Flags
CC = gcc
STD = -std=c17
CFLAGS = -Wall -Wextra 
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-parameter

CFLAGS += $(STD)
CFLAGS += -g

RELEASE_CFLAGS = -Wall $(STD) -O2 -DNDEBUG
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
SRC_HEADERS=$(wildcard $(SRC_DIR)/*.h)
INC_HEADERS=$(wildcard $(INC_DIR)/*.h)
OBJS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DFLAGS := $(addprefix -D,$(DFLAGS))

all: debug ## Default rule

b: build
build: debug ## Build current target

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(TARGET): $(OBJS) headers
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET) -I$(INC_DIR) -L$(LIB_DIR) -l:$(LIBFXC)

headers: $(SRC_HEADERS) $(INC_HEADERS)

db: debug ##
debug: clean
debug: makedirs Makefile
debug: CFLAGS += $(DFLAGS) # set -D flags
debug: $(TARGET) ## Build debug target with -D flags

ndb: ##
nodebug: DFLAGS := # unset -D flags
nodebug: clean debug ## Build debug target without -D flags

release: CFLAGS := $(RELEASE_CFLAGS)
release: clean
release: $(TARGET) ## Build release target

rb: rebuild ##
rebuild: clean build ## Clean and rebuild target

r: run ##
run: ## Run current target
	./$(BIN_DIR)/$(TARGET)

rndb: ndb run
rdb: db run

c: clean ##
clean: ## Clean up build directories
	$(RM) $(OBJ_DIR)/* $(BIN_DIR)/*

makedirs: ## Create build directories
	@mkdir -p $(INC_DIR) $(LIB_DIR) $(OBJ_DIR) $(BIN_DIR)

libs: src_fxlib ## Source external libraries

src_fxlib:
	mkdir -p  $(LIBFXC_INC_DIR)
	$(RM) $(LIB_DIR)/* $(LIBFXC_INC_DIR)/*
	cp -fr $(LIBFXC_SRC_DIR)/include/* $(LIBFXC_INC_DIR)
	cp -fr $(LIBFXC_SRC_DIR)/lib/* $(LIB_DIR)

check: ## Run valgrind memory sanitizer
	@valgrind --undef-value-errors=no  $(BIN_DIR)/$(TARGET)

format: ## Format with clang-format
	@clang-format -i $(SRCS)

h: help ##
help: ## Show this message
	@awk 'BEGIN {FS = ":.*##"; printf "Usage:\n  make  \033[36m<target>\033[0m\n\nTargets:\n"} \
    /^[a-zA-Z_-]+:.*?##/ { if(length($$2) == 0 ) { printf "\033[36m%7s\033[0m", $$1 } \
							  else { printf "\t\033[36m%-10s\033[0m %s\n", $$1, $$2 }}' $(MAKEFILE_LIST)

.PHONY: help run build rebuild release debug makedirs clean check source_libs
