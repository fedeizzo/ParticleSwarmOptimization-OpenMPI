# COMPILER
CC := gcc # compiler
CPPFLAGS := -Iinclude -MMD -MP # preprocessor flags
CFLAGS := -g -Wall # compiler flags
LDFLAGS := -Llib # linker flags
LDLIBS :=

# STANDARD DIRECTORIES
SRC_DIR := src
DIRS    := $(notdir $(filter-out src/main.c, $(wildcard $(SRC_DIR)/*)))
OBJ_DIR := obj
BIN_DIR := bin
INC_DIR := include

# TARGET
TARGET := $(BIN_DIR)/hpc_test

# FILES
SRC := $(wildcard $(SRC_DIR)/**/*c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
DEPS := $(wildcard $(INC_DIR)/*.h)

# COMMANDS
MKDIR := mkdir -p
RM := rm
ECHO := echo -e

# COLORS
RED := \033[31m
GREEN := \033[32m
YELLOW := \033[33m
BLUE := \033[34m
NONE := \033[0m

# RULES

# all
all: clean build
# phony: not targets
.PHONY: all clean help build

build: $(TARGET)

# building target, dealing with the linking phase, the compiling is over
$(TARGET): $(OBJ) | $(BIN_DIR)
	$(ECHO) "$(GREEN)Program compiled successfully$(NONE)";
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(ECHO) "$(GREEN)Program linked successfully$(NONE)";

# building objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS) | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# create object folder if not present yet
$(BIN_DIR) $(OBJ_DIR):
	$(MKDIR) $@
	$(foreach dir,$(DIRS),$(MKDIR) $@/$(dir);)

clean:
	@$(RM) -rf $(BIN_DIR) $(OBJ_DIR)
	@$(ECHO) "$(YELLOW)Artifacts cleaned successfully$(NONE)";

help: 
	@$(ECHO) "$(BLUE)Makefile help\n \
	* build    : compiles the program and creates the object files and the executable files\n \
	* clean    : removes all the object and binary files\n \
	* all      : clean and then compiles$(NONE)";

-include $(OBJ:.o=.d) # The dash is used to silence errors if the files don't exist yet
