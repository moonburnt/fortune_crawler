SRC_DIR = src
BUILD_DIR = build
ASSETS_DIR = Assets
MAPS_DIR = maps
GAME_NAME = fortune_crawler

CC = g++
DEPS = json-c raylib fmt
CFLAGS = -Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -std=c++17 $(shell pkg-config --cflags $(DEPS))
LDFLAGS = -lm $(shell pkg-config --libs $(DEPS))

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
HEADERS := $(SRC_DIR)/*.hpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: dir $(BUILD_DIR)/$(GAME_NAME)
	cp -r $(ASSETS_DIR) $(BUILD_DIR)
	cp -r $(MAPS_DIR) $(BUILD_DIR)

debug: CFLAGS += -ggdb -O0
debug: all

asan: CFLAGS += -ggdb -O0 -fsanitize=address
asan: LDFLAGS += -fsanitize=address
asan: all

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(GAME_NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

dir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

check-format:
	find src \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -style=file -n {} \;

format:
	find src \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -style=file -i {} \;

.PHONY: dir clean format check-format debug
