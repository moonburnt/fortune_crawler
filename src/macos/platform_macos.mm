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

#include "platform_macos.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include <fmt/format.h>

std::string PlatformMacos::get_resource_dir() {
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef resource_dir = CFBundleCopyResourcesDirectoryURL(bundle);
    CFURLRef path = CFURLCopyAbsoluteURL(resource_dir);
    CFStringRef string = CFURLCopyFileSystemPath(path, kCFURLPOSIXPathStyle);
    char bundle_str [4096] = { 0 };
    CFStringGetCString(string, bundle_str, sizeof(bundle_str), kCFStringEncodingUTF8);
    CFRelease(string);
    CFRelease(path);
    CFRelease(resource_dir);
    return std::string(bundle_str) + "/";
}

std::string PlatformMacos::get_maps_dir() {
    return get_resource_dir();
}

std::string PlatformMacos::get_sprites_dir() {
    return get_resource_dir();
}

std::string PlatformMacos::get_sounds_dir() {
    return get_resource_dir();
}

std::string PlatformMacos::get_music_dir() {
    return get_resource_dir();
}

std::string PlatformMacos::get_settings_dir() {
    return get_resource_dir();
}
