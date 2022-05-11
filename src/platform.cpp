// Fortune Crawler - a conceptual dungeon crawler with rock-paper-scissors events.
// Copyright (c) 2022 moonburnt
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see https://www.gnu.org/licenses/gpl-3.0.txt

#include "platform.hpp"

#if defined(PLATFORM_MACOS)
#include "macos/platform_macos.h"
#elif defined(PLATFORM_LINUX)
#include "linux/platform_linux.hpp"
#elif defined(PLATFORM_WINDOWS)
#include "windows/platform_windows.hpp"
#else
#error "Platform is not defined or could not be detected"
#endif

std::unique_ptr<Platform> Platform::make_platform() {
#if defined(PLATFORM_MACOS)
    return std::make_unique<PlatformMacos>();
#elif defined(PLATFORM_LINUX)
    return std::make_unique<PlatformLinux>();
#elif defined(PLATFORM_WINDOWS)
    return std::make_unique<PlatformWindows>();
#else
#error "Platform is not defined or could not be detected"
#endif
}
