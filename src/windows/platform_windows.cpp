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

#include "platform_windows.hpp"

std::string PlatformWindows::get_resource_dir() {
    return "../Assets/";
}

std::string PlatformWindows::get_maps_dir() {
    return "../maps/";
}

std::string PlatformWindows::get_sprites_dir() {
    return get_resource_dir() + "Sprites/";
}

std::string PlatformWindows::get_sounds_dir() {
    return get_resource_dir() + "SFX/";
}

std::string PlatformWindows::get_music_dir() {
    return get_resource_dir() + "Music/";
}

std::string PlatformWindows::get_settings_dir() {
    return "./";
}
