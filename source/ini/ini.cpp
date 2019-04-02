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

#include <fstream>
#include <iostream>
#include "ini/simpleIniParser/ini.hpp"
#include "ini/simpleIniParser/ini_option.hpp"
#include "trim.hpp"

using namespace std;

namespace simpleIniParser {
    Ini::~Ini() {
        for (IniSection * section : sections) {
            if (section != nullptr) {
                delete section;
                section = nullptr;
            }
        }
        sections.clear();
    }

    string Ini::build() {
        string result;

        for (IniSection * section : sections) {
            result += section->build() + "\n";
        }

        return result;
    }

    IniSection * Ini::findSection(string name) {
        auto it = find_if(sections.begin(), sections.end(), [&name](const IniSection * obj) { return obj->value == name; });
        if (it == sections.end())
            return nullptr;

        return (*it);
    }

    bool Ini::writeToFile(string path) {
        ofstream file(path);
        if (!file.is_open())
            return false;

        file << build();

        file.close();

        return true;
    }

    Ini * Ini::parseFile(string path) {
        ifstream file(path);
        if (!file.is_open())
            return nullptr;

        Ini * ini = new Ini();
        string line;
        while (getline(file, line)) {
            trim(line);

            if (line.size() == 0)
                continue;

            IniSection * section = IniSection::parse(line);
            if (section != nullptr) {
                ini->sections.push_back(section);
            }
            else if (ini->sections.size() != 0 && ini->sections.back()->type == SECTION) {
                IniOption * option = IniOption::parse(line);

                if (option != nullptr)
                    ini->sections.back()->options.push_back(option);
            }
        }

        file.close();

        return ini;
    }
}
