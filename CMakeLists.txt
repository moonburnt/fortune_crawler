cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Directory to build executable into
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/game")

# set project name and version
project(Game
    LANGUAGES CXX
    VERSION 0.1
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("---- RELEASE MODE ----")
    add_definitions(-O2)
else()
    message("---- DEBUG MODE ----")
    add_definitions(-DDEBUG -O0 -g)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer new GLVND over OLD
set(OpenGL_GL_PREFERENCE GLVND)

add_executable(Game
    src/shared.cpp
    src/shared.hpp
    src/common.cpp
    src/common.hpp
    src/entity.cpp
    src/entity.hpp
    src/event_screens.cpp
    src/event_screens.hpp
    src/loader.cpp
    src/loader.hpp
    src/level.cpp
    src/level.hpp
    src/mapgen.cpp
    src/mapgen.hpp
    src/menus.cpp
    src/menus.hpp
    src/main.cpp
)

target_include_directories(Game
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)

# This is basically the same as add_definitions(), but to be applied only to Game.
target_compile_options(Game
    PUBLIC
    -Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -Wno-missing-field-initializers
)

# Setup engine
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/engine")
target_link_libraries(Game engine)
target_include_directories(Game PUBLIC ${engine_INCLUDE_DIRS})

# Setup nlohmann_json
add_library(
    nlohmann_json
    "${CMAKE_SOURCE_DIR}/dependencies/nlohmann_json/single_include"
)
set_target_properties(nlohmann_json PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(nlohmann_json PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/nlohmann_json/single_include")
target_link_libraries(Game nlohmann_json)
target_include_directories(Game PUBLIC ${nlohmann_json_INCLUDE_DIRS})


# Custom command to be used after Game is built
# This should copy assets and maps into the directory with game's binary
add_custom_command(TARGET Game POST_BUILD
    COMMAND cp -r ./Assets ./maps "${CMAKE_BINARY_DIR}/game"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Copying assets to ${CMAKE_BINARY_DIR}/game"
)


# Custom targets to be used instead of --build
add_custom_target(compile_commands
    WORKING_DIRECTORY "${CMAKE_BUILD_DIR}"
    BYPRODUCTS ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMAND cp -f ./compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
    VERBATIM)

add_custom_target(format
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND find src \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -style=file -i {} \;
    VERBATIM)
