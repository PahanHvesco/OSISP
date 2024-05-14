CC = gcc
DEBUG_FLAGS = -g -ggdb -std=c11 -pedantic -W -Wall -Wextra
RELEASE_FLAGS = -std=c11 -pedantic -W -Wall -Wextra -Werror
SRC_DIR = src
BUILD_DIR = build
DEBUG_BUILD_DIR = $(BUILD_DIR)/debug
RELEASE_BUILD_DIR = $(BUILD_DIR)/release
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
HEADER_FILES = $(wildcard $(SRC_DIR)/*.h)
DEBUG_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(DEBUG_BUILD_DIR)/%.o,$(SRC_FILES))
RELEASE_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(RELEASE_BUILD_DIR)/%.o,$(SRC_FILES))
DEBUG_EXEC = $(BUILD_DIR)/debug/backup
RELEASE_EXEC = $(BUILD_DIR)/release/backup

.PHONY: all clean

all: debug release

debug: $(DEBUG_EXEC)

release: $(RELEASE_EXEC)

$(DEBUG_BUILD_DIR):
	mkdir -p $@

$(RELEASE_BUILD_DIR):
	mkdir -p $@

$(DEBUG_BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_FILES)
	$(CC) $(DEBUG_FLAGS) -c $< -o $@

$(RELEASE_BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_FILES)
	$(CC) $(RELEASE_FLAGS) -c $< -o $@

$(DEBUG_EXEC): $(DEBUG_BUILD_DIR) $(DEBUG_OBJ_FILES)
	$(CC) $(DEBUG_FLAGS) $(DEBUG_OBJ_FILES) -o $@

$(RELEASE_EXEC): $(RELEASE_BUILD_DIR) $(RELEASE_OBJ_FILES)
	$(CC) $(RELEASE_FLAGS) $(RELEASE_OBJ_FILES) -o $@

clean:
	rm -rf $(BUILD_DIR)
