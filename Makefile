# NOTE, for the pkg-config you might require to add yours in the path variable 
# export PKG_CONFIG_PATH="/usr/lib64/pkgconfig:$PKG_CONFIG_PATH"

# DOCKER
DOCKER_TAG :=

# LIBRARIES
LIBRARIES         := sqlite3 check
LINKING_LIBRARIES :=

# COMPILER
CC        := mpicc # compiler
CPPFLAGS  := -O3 -Iinclude -lm -MMD -MP $(foreach pkg, ${LIBRARIES}, $(shell pkg-config --cflags ${pkg})) # preprocessor flags
CFLAGS    := -O3 -fopenmp -g -Wall -lm # compiler flags
LDFLAGS   := -O3 -fopenmp -Llib -lm # linker flags
LDLIBS    := $(foreach pkg, ${LIBRARIES}, $(shell pkg-config --libs ${pkg}))
TESTFLAGS := -lcheck

# DOXYGEN
DOXYGEN       := doxygen
DOXYGEN_CONF  := -w html
DOXYFILE      := Doxyfile
DOXYGEN_INDEX := html/index.html

# STANDARD DIRECTORIES
SRC_DIR      := src
TEST_DIR     := tests
DIRS         := $(notdir $(wildcard $(SRC_DIR)/*))
OBJ_DIR      := obj
BIN_DIR      := bin
INC_DIR      := include
DOC_DIR      := docs

# TARGET
TARGET      := $(BIN_DIR)/particle-swarm-optimization
TEST_TARGET := $(BIN_DIR)/test

# FILES
SRC       := $(shell find $(SRC_DIR) -name '*.c')
TESTS     := $(shell find $(TEST_DIR) -name '*.c')
OBJ       := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
TEST_OBJ  := $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TESTS)) $(filter-out $(OBJ_DIR)/main.o,$(OBJ))
DEPS      := $(wildcard $(INC_DIR)/*.h)

# COMMANDS
MKDIR := mkdir -p
RM    := rm
ECHO  := echo -e
OPEN  := xdg-open

# COLORS
RED    := \033[31m
GREEN  := \033[32m
YELLOW := \033[33m
BLUE   := \033[34m
NONE   := \033[0m

# RULES

# all
all: clean test build
# phony: not targets
.PHONY: all clean help test build report

build: $(TARGET)
test: $(TEST_TARGET)

# building target, dealing with the linking phase, the compiling is over
$(TARGET): $(OBJ) | $(BIN_DIR)
	@$(ECHO) "$(GREEN)Program compiled successfully$(NONE)";
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@ -lm $(LINKING_LIBRARIES)
	@$(ECHO) "$(GREEN)Program linked successfully$(NONE)";

# building objects
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c $(DEPS) | $(OBJ_DIR)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS) | $(OBJ_DIR)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# create object folder if not present yet
$(BIN_DIR) $(OBJ_DIR):
	@$(MKDIR) $@
	@$(foreach dir,$(DIRS),$(MKDIR) $@/$(dir);)

$(TEST_TARGET): $(TEST_OBJ) | $(BIN_DIR)
	@$(ECHO) "$(GREEN)Program compiled successfully for tests$(NONE)";
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) $(TESTFLAGS) -o $@ -lm $(LINKING_LIBRARIES)
	@$(ECHO) "$(GREEN)Program linked successfully for tests$(NONE)";
	@$(TEST_TARGET)

clean:
	@$(RM) -rf $(BIN_DIR) $(OBJ_DIR) $(TEST_OBJ_DIR)
	@$(ECHO) "$(YELLOW)Artifacts cleaned successfully$(NONE)";

doc:
	@$(MKDIR) $(DOC_DIR)
	@$(DOXYGEN) $(DOXYFILE) $(DOXYGEN_CONF)

report:
	@./scripts/generate_report.sh

cluster-run:
	@./scripts/generate_cluster_runs.sh

cluster-pull:
	@./scripts/build.sh --cluster

cluster-clear:
	@qstat -u $USER | tail -n +6 | awk -F' ' '{print $1}' | awk -F '.' '{print $1}' | xargs qdel

docker-build:
	@./scripts/build.sh $(DOCKER_TAG)

open-doc:
	@$(OPEN) $(DOC_DIR)/$(DOXYGEN_INDEX)

help: 
	@$(ECHO) "$(BLUE)Makefile help\n \
	* build         : compiles the program and creates the object files and the executable files\n \
	* test          : compiles tests and creates the object files and the executable test files\n \
	* clean         : removes all the object and binary files\n \
	* doc           : generates the code documentation in HTML\n \
	* report        : generates pdf report\n \
	* cluster-run   : launches multiple qsub runs\n \
	* cluster-pull  : pulls latest udocker container\n \
	* cluster-clear : deletes all submitted jobs\n \
	* docker-build  : builds latest docker container\n \
	* open-doc      : compiles and then opens the HTML documentation\n \
	* all           : clean and then compiles$(NONE)";

-include $(OBJ:.o=.d) # The dash is used to silence errors if the files don't exist yet
