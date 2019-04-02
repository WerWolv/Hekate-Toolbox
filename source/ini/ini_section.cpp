// SimpleIniParser
// Copyright (C) 2019 Steven Mattera
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ini/simpleIniParser/ini_section.hpp"
#include <algorithm>

using namespace std;

namespace simpleIniParser {
    IniSection::IniSection(IniSectionType t, string v) {
        type = t;
        value = v;
    }

    IniSection::~IniSection() {
        for (IniOption * option : options) {
            if (option != nullptr) {
                delete option;
                option = nullptr;
            }
        }
        options.clear();
    }

    IniOption * IniSection::findFirstOption(string key) {
        auto it = find_if(options.begin(), options.end(), [&key](const IniOption * obj) { return obj->key == key; });
        if (it == options.end())
            return nullptr;

        return (*it);
    }

    string IniSection::build() {
        if (type == HEKATE_CAPTION)
            return "{" + value + "}\n";
            
        if (type == SEMICOLON_COMMENT)
            return ";" + value + "\n";
            
        if (type == HASHTAG_COMMENT)
            return "#" + value + "\n";

        if (type == BLANK_LINE)
            return "\n";

        string result = "[" + value + "]\n";

        for (IniOption * option : options) {
            result += option->build() + "\n";
        }

        return result;
    }

    IniSection * IniSection::parse(string line) {
        if (line.at(0) == '{' && line.at(line.size() - 1) == '}') {
            return new IniSection(HEKATE_CAPTION, line.substr(1, line.size() - 2));
        } else if (line.at(0) == ';') {
            return new IniSection(SEMICOLON_COMMENT, line.substr(1, line.size() - 1));
        } else if (line.at(0) == '#') {
            return new IniSection(HASHTAG_COMMENT, line.substr(1, line.size() - 1));
        } else if (line.at(0) == '[' && line.at(line.size() - 1) == ']') {
            return new IniSection(SECTION, line.substr(1, line.size() - 2));
        } else if (line.size() == 0) {
            return new IniSection(BLANK_LINE, "");
        } else {
            return nullptr;
        }
    }
}
