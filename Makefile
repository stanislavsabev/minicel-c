# Compiler Flags
CC = gcc
STD = -std=c99
CFLAGS = -Wall -Wextra -std=c99 -fPIC -g3 -O0 -I.

DFLAGS = DEBUG

# Directories
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Make sure output directories exist
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# Target name
TARGET = minicell

SRCS=$(wildcard $(SRC_DIR)/*.c)
SRC_HEADERS=$(wildcard $(SRC_DIR)/*.h)

OBJS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DFLAGS := $(addprefix -D,$(DFLAGS))


h: help ##
help: ## Show this message
	@awk 'BEGIN {FS = ":.*##"; printf "Usage:\n  make  \033[36m<target>\033[0m\n\nTargets:\n"} \
    /^[a-zA-Z_-]+:.*?##/ { if(length($$2) == 0 ) { printf "\033[36m%7s\033[0m", $$1 } \
							  else { printf "\t\033[36m%-10s\033[0m %s\n", $$1, $$2 }}' $(MAKEFILE_LIST)


b: build ##
build: debug ## Build current target

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(TARGET): $(OBJS) $(SRC_HEADERS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET)


db: debug
debug: makedirs Makefile
debug: CFLAGS += $(DFLAGS) # set -D flags
debug: $(TARGET)


rb: rebuild ##
rebuild: clean build ## Clean and rebuild target


r: run ##
run: build ## Run current target
	@$(BIN_DIR)/$(TARGET) ./input.csv


c: clean ##
clean: ## Clean up build directories
	$(RM) $(OBJ_DIR)/* $(BIN_DIR)/*


sy: sync ##
sync: ## Sync function declarations with implementations
	python scripts/sync_declarations.py src/fx.h 


f: format ##
format: ## Format with clang-format
	@clang-format -i $(SRCS) $(SRC_HEADERS)


makedirs: ## Create build directories
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)


check: ## Run valgrind memory sanitizer
	@valgrind --undef-value-errors=no  $(BIN_DIR)/$(TARGET)
