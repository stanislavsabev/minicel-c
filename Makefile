# Compiler Flags
CC = gcc
STD = -std=c11
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

.PHONY: all
all:
	@echo "Missing target. Use 'make help' to see details."

.PHONY: b build
b: build ##
build: makedirs $(TARGET) ## Build current target

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(TARGET): $(OBJS) headers
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET)

.PHONY: headers
headers: $(SRC_HEADERS)

.PHONY: rb rebuild
rb: rebuild ##
rebuild: clean build ## Clean and rebuild target

.PHONY: r run
r: run ##
run: ## Run current target
	@$(BIN_DIR)/$(TARGET) ./input.csv

.PHONY: br rbr
br: build run ## Build and run
rbr: rebuild run ## Rebuild and run

.PHONY: c clean
c: clean ##
clean: ## Clean up build directories
	$(RM) $(OBJ_DIR)/* $(BIN_DIR)/*

.PHONY: md makedirs
md: makedirs ##
makedirs: ## Create build directories
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

.PHONY: f format
f: format ##
format: ## Format with clang-format
	@clang-format -i $(SRCS) $(SRC_HEADERS)

.PHONY: h help
h: help ##
help: ## Show this message
	@awk 'BEGIN {FS = ":.*##"; printf "Usage:\n  make  \033[36m<target>\033[0m\n\nTargets:\n"} \
    /^[a-zA-Z_-]+:.*?##/ { if(length($$2) == 0 ) { printf "\033[36m%7s\033[0m", $$1 } \
							  else { printf "\t\033[36m%-10s\033[0m %s\n", $$1, $$2 }}' $(MAKEFILE_LIST)
