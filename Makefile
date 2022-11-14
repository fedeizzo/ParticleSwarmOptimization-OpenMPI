# NOTE, for the pkg-config you might require to add yours in the path variable 
# export PKG_CONFIG_PATH="/usr/lib64/pkgconfig:$PKG_CONFIG_PATH"

# LIBRARIES
LIBRARIES := sqlite3

# COMPILER
CC := mpicc # compiler
CPPFLAGS := -Iinclude -lm -MMD -MP $(foreach pkg, ${LIBRARIES}, $(shell pkg-config --cflags ${pkg})) # preprocessor flags
CFLAGS := -fopenmp -g -Wall -lm # compiler flags
LDFLAGS := -fopenmp -Llib -lm # linker flags
LDLIBS := $(foreach pkg, ${LIBRARIES}, $(shell pkg-config --libs ${pkg}))

# DOXYGEN
DOXYGEN := doxygen
DOXYGEN_CONF := -w html
DOXYFILE := Doxyfile
DOXYGEN_INDEX := html/index.html

# STANDARD DIRECTORIES
SRC_DIR := src
DIRS    := $(notdir $(wildcard $(SRC_DIR)/*))
OBJ_DIR := obj
BIN_DIR := bin
INC_DIR := include
DOC_DIR := docs

# TARGET
TARGET := $(BIN_DIR)/particle-swarm-optimization

# FILES
SRC := $(shell find $(SRC_DIR) -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
DEPS := $(wildcard $(INC_DIR)/*.h)

# COMMANDS
MKDIR := mkdir -p
RM := rm
ECHO := echo -e
OPEN := xdg-open

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
	@$(ECHO) "$(GREEN)Program compiled successfully$(NONE)";
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@ -lm
	@$(ECHO) "$(GREEN)Program linked successfully$(NONE)";

# building objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS) | $(OBJ_DIR)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# create object folder if not present yet
$(BIN_DIR) $(OBJ_DIR):
	@$(MKDIR) $@
	@$(foreach dir,$(DIRS),$(MKDIR) $@/$(dir);)

clean:
	@$(RM) -rf $(BIN_DIR) $(OBJ_DIR)
	@$(ECHO) "$(YELLOW)Artifacts cleaned successfully$(NONE)";

doc:
	@$(MKDIR) $(DOC_DIR)
	@$(DOXYGEN) $(DOXYFILE) $(DOXYGEN_CONF)

open-doc:
	@$(OPEN) $(DOC_DIR)/$(DOXYGEN_INDEX)

help: 
	@$(ECHO) "$(BLUE)Makefile help\n \
	* build    : compiles the program and creates the object files and the executable files\n \
	* clean    : removes all the object and binary files\n \
	* doc      : generates the code documentation in HTML\n \
	* open-doc : compiles and then opens the HTML documentation\n \
	* all      : clean and then compiles$(NONE)";

-include $(OBJ:.o=.d) # The dash is used to silence errors if the files don't exist yet
