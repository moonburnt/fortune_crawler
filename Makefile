SRC_DIR = src
BUILD_DIR = build
ASSETS_DIR = Assets
GAME_NAME = luck_crawler

CC = g++
CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c++17
LDFLAGS = -lm -lraylib -ljson-c

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
HEADERS := $(SRC_DIR)/*.hpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: dir $(BUILD_DIR)/$(GAME_NAME)
	cp -r $(ASSETS_DIR) $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(GAME_NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

dir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: dir clean
