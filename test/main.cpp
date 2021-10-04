// ItunesLibraryParser
// Allows JSON parsing of XML Itunes Library file
// Copyright (C) 2021 Guillaume Vara <guillaume.vara@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Graphical resources without explicit references to a
// different license and copyright still refer to this GPL.

#include "ITunesLibraryParser.hpp"
#include "Mesurable.hpp"

int main() {
    // make measurements more precise
    Measurable::makeMorePrecise();

    auto m = Measurable { "Parse XML ITunes Library File into JSON" };

        const auto parser = ITunesLibraryParser {
            "iTunes Music Library.xml",
            "output.json",
            "warnings.json"
        };

        parser.produceOutputs();

    m.printElapsed();

    return 0;
}
