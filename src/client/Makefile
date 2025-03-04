# Executable name
EXEC_NAME = ui.elf

# Folder structure
SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin

# Executable path
EXEC = $(BIN_DIR)/$(EXEC_NAME)

# Commands
RM = rm -f
CC = gcc
MKDIR_P = mkdir -p

# Compilation flags
CCFLAGS = -Wall -MMD -MP
LDFLAGS = -lm

# Error handeling 
#CCFLAGS += -Werror=uninitialized

# Source files
SRC = $(wildcard $(shell find $(SRC_DIR) -type f -regex ".*\.c"))

# Object files
OBJ = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRC:.c=.o))
DEP = $(OBJ:.o=.d)

# Compilation rules
all: $(EXEC)

# Link the object files to create the executable
$(EXEC): $(OBJ)
	@ echo "\n==== Linker ========="
	@ $(MKDIR_P) $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)
	@ echo "==== Linker done ====\n"

# Dependances
-include $(DEP)

# Compile the source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@ echo "\n==== Build Object $< ========="
	@ $(MKDIR_P) $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@
	@ echo "==== Build Object done $< ====\n"

# Clean the project
clean:
	@ $(RM) -r $(BUILD_DIR) $(BIN_DIR)
	@ echo ">>> Build files cleaned"

hello: 
	@ echo "Hello World"

# Phony targets
.PHONY: all clean