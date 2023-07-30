CC=clang
STD=-std=c17
CFLAGS=-Wall -Wextra -g $(STD)

SRC=src
OBJ=obj
BIN=bin
TARGET=main

SRCS=$(wildcard $(SRC)/*.c)
HSRCS=$(wildcard $(SRC)/*.h)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

.PHONY: all
all: build

.PHONY: help
help: ## Show this message
	@awk 'BEGIN {FS = ":.*##"; printf "Usage:\n  make \033[36m<target>\033[0m\n\nTargets:\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 }' $(MAKEFILE_LIST)

.PHONY: dirs
dirs: ## Create build directories
	@mkdir -p $(OBJ) $(BIN)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(TARGET): $(OBJS) $(HSRCS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN)/$(TARGET)

.PHONY: build
build: dirs Makefile $(TARGET) ## Build target

.PHONY: rebuild
rebuild: clean build ## Cleanup and build target

.PHONY: run
run: $(TARGET) ## Run target
	./$(BIN)/$(TARGET)

.PHONY: clean ## Clean up build directories
clean: ## Clean up
	$(RM) $(OBJ)/* $(BIN)/*

.PHONY: release
release: CFLAGS=-Wall $(STD) -O2 -DNDEBUG
release: clean
release: $(TARGET)

.PHONY: check
check:
	@valgrind --undef-value-errors=no  ./$(BIN)/$(TARGET)