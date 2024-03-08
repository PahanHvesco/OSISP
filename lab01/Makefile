CC := gcc
CFLAGS_common := -std=gnu99 -pedantic -W -Wall -Wextra
SRC_DIR := src
BUILD_DIR := build
DEBUG_DIR := $(BUILD_DIR)/debug
RELEASE_DIR := $(BUILD_DIR)/release

SRC := $(wildcard $(SRC_DIR)/*.c)
DEBUG_OBJ := $(patsubst $(SRC_DIR)/%,$(DEBUG_DIR)/%,$(SRC:.c=.o))
RELEASE_OBJ := $(patsubst $(SRC_DIR)/%,$(RELEASE_DIR)/%,$(SRC:.c=.o))

TARGET := app

.PHONY: all debug release clean

all: debug release

debug: CFLAGS := -g -ggdb $(CFLAGS_common)
debug: $(DEBUG_DIR)/$(TARGET)

release: CFLAGS := $(CFLAGS_common) -O2 -DNDEBUG
release: $(RELEASE_DIR)/$(TARGET)

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(DEBUG_DIR)/$(TARGET): $(DEBUG_OBJ)
	mkdir -p $(DEBUG_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(RELEASE_DIR)/$(TARGET): $(RELEASE_OBJ)
	mkdir -p $(RELEASE_DIR)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(BUILD_DIR)
