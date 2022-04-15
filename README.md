# Fortune Crawler

## Description

**Fortune Crawler** - conceptual dungeon crawler where every event is solved via
rock-paper-scissors minigame.

## Pre-requirements

- CMake 3.10+
- clang++ 13.0.1

## Installation

```
git submodule update --init
git -C dependencies/engine submodule update --init
mkdir ./build
cmake . -B ./build
cmake --build ./build
```

Build files and game's executable will be generated into ./build directory
