# Fortune Crawler

[![Linux Build](https://github.com/moonburnt/fortune_crawler/actions/workflows/build_linux.yml/badge.svg)](https://github.com/moonburnt/fortune_crawler/actions/workflows/build_linux.yml) [![Windows Build](https://github.com/moonburnt/fortune_crawler/actions/workflows/build_windows.yml/badge.svg)](https://github.com/moonburnt/fortune_crawler/actions/workflows/build_windows.yml)

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

Build files will be generated into ./build directory, and Game executable - into
./build/game (assets will be copied there too)

## LICENSE

[GPLv3](https://github.com/moonburnt/fortune_crawler/blob/master/LICENSE)

For licenses of individual media files used in this game - check *.txt files in
Assets directory.
