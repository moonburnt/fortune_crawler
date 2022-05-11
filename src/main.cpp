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

#include "app.hpp"

#include <spdlog/spdlog.h>

int main(int argc, char* const* argv) {
    // Processing launch arguments.
    // For now there is just one - to toggle on debug messages.
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (std::strcmp(argv[i], "--debug") == 0) {
                spdlog::set_level(spdlog::level::debug);
            }
        }
    }

    App app;
    app.run();

    return 0;
}
