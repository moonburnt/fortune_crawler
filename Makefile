SRC_DIR = src
BUILD_DIR = build
ASSETS_DIR = Assets
GAME_NAME = fortune_crawler

CC = g++
DEPS = json-c raylib
CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c++17 $(shell pkg-config --cflags $(DEPS))
LDFLAGS = -lm $(shell pkg-config --libs $(DEPS))

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
