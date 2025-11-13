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
OBJ_DIR = ./obj
BIN_DIR = ./bin


# Target name
TARGET = minicell

SRCS=$(wildcard $(SRC_DIR)/*.c)
SRC_HEADERS=$(wildcard $(SRC_DIR)/*.h)

OBJS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DFLAGS := $(addprefix -D,$(DFLAGS))

.PHONY: default_
default_:
	@echo "Missing target. Use 'make help' to see details."

.PHONY: b build
b: build ##
build: makedirs Makefile ## Build current target

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(TARGET): $(OBJS) headers
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET) -I$(INC_DIR) -L$(LIB_DIR) -l:$(LIBFXC)

.PHONY: headers
headers: $(SRC_HEADERS) $(INC_HEADERS)

.PHONY: rb rebuild
rb: rebuild ##
rebuild: clean build ## Clean and rebuild target

.PHONY: r run
r: run ##
run: ## Run current target
	@$(BIN_DIR)/$(TARGET) ./input.csv

.PHONY: c clean
c: clean ##
clean: ## Clean up build directories
	$(RM) $(OBJ_DIR)/* $(BIN_DIR)/*

makedirs: ## Create build directories
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

.PHONY: f format
f: format ##
format: ## Format with clang-format
	@clang-format -i $(SRCS)

.PHONY: h help
h: help ##
help: ## Show this message
	@awk 'BEGIN {FS = ":.*##"; printf "Usage:\n  make  \033[36m<target>\033[0m\n\nTargets:\n"} \
    /^[a-zA-Z_-]+:.*?##/ { if(length($$2) == 0 ) { printf "\033[36m%7s\033[0m", $$1 } \
							  else { printf "\t\033[36m%-10s\033[0m %s\n", $$1, $$2 }}' $(MAKEFILE_LIST)
