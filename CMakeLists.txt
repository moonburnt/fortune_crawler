cmake_minimum_required(VERSION 3.21)

if(PROJECT_IS_TOP_LEVEL)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
    endif()
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Directory to build executable into
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/game")

# set project name and version
project(Game
    LANGUAGES CXX
    VERSION 0.1
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer new GLVND over OLD
set(OpenGL_GL_PREFERENCE GLVND)

add_executable(Game)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(GLOB RESOURCES
        "Assets/SFX/*.ogg"
        "Assets/Sprites/*.png"
        "maps/*.png"
    )

    target_sources(Game PRIVATE
        src/macos/platform_macos.mm
        src/macos/platform_macos.h
        ${RESOURCES})

    target_compile_definitions(Game PRIVATE
        "PLATFORM_MACOS"
    )

    set_target_properties(Game PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/src/macos/Info.plist.in"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER com.moonburnt.fortune-crawler
        XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES YES
        XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION 0.0.1
        XCODE_ATTRIBUTE_MARKETING_VERSION 0.0.1
        RESOURCE "${RESOURCES}"
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(Game PRIVATE
        src/windows/platform_windows.hpp
        src/windows/platform_windows.cpp
    )

    target_compile_definitions(Game PRIVATE
        "PLATFORM_WINDOWS"
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(Game PRIVATE
        src/linux/platform_linux.hpp
        src/linux/platform_linux.cpp
    )

    target_compile_definitions(Game PRIVATE
        "PLATFORM_LINUX"
    )
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

target_sources(Game PRIVATE
    src/app.cpp
    src/app.hpp
    src/event_screens.cpp
    src/event_screens.hpp
    src/common.cpp
    src/common.hpp
    src/entity.cpp
    src/entity.hpp
    src/level.cpp
    src/level.hpp
    src/loader.cpp
    src/loader.hpp
    src/menus.cpp
    src/menus.hpp
    src/mapgen.cpp
    src/mapgen.hpp
    src/main.cpp
    src/platform.hpp
    src/platform.cpp
)

target_compile_options(Game PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -Wno-missing-field-initializers>
    $<$<CXX_COMPILER_ID:MSVC>:/Wall /w34263 /w34266>
    $<$<AND:$<CONFIG:Debug>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-fsanitize=address>
)

target_link_options(Game PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-fsanitize=address>)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Custom command to be used after Game is built
    # This should copy assets into the directory with game's binary
    add_custom_command(TARGET Game POST_BUILD
        COMMAND cp -r ./Assets ./maps "${CMAKE_BINARY_DIR}/game"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Copying assets to ${CMAKE_BINARY_DIR}/game"
    )
endif()

target_include_directories(Game PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)

# Setup engine
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/engine")
target_link_libraries(Game engine)
target_include_directories(Game PRIVATE ${engine_INCLUDE_DIRS})

# Setup nlohmann_json
add_library(
    nlohmann_json
    "${CMAKE_SOURCE_DIR}/dependencies/nlohmann_json/single_include"
)
set_target_properties(nlohmann_json PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(nlohmann_json PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/nlohmann_json/single_include")
target_link_libraries(Game nlohmann_json)
target_include_directories(Game PRIVATE ${nlohmann_json_INCLUDE_DIRS})


# Custom targets to be used instead of --build
add_custom_target(compile_commands
  WORKING_DIRECTORY ${CMAKE_BUILD_DIR}
  BYPRODUCTS ${CMAKE_SOURCE_DIR}/compile_commands.json
  COMMAND cp -f ./compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
  VERBATIM)

add_custom_target(format
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND find src \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -style=file -i {} \;
  VERBATIM)
