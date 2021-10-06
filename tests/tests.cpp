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

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "ITunesLibraryParser.hpp"
#include "Mesurable.hpp"

bool parsing_succeeded(const std::string &fileName) {
    // make measurements more precise
    Measurable::makeMorePrecise();

    auto m = Measurable { "Parse XML ITunes Library File into JSON" };

        const auto parser = ITunesLibraryParser {
            fileName.c_str(),
            "output.json",
            "warnings.json"
        };

        parser.produceOutputs();

    m.printElapsed();

    return true;
}

TEST_CASE("Parse small file", "[parser]") {
    REQUIRE(parsing_succeeded("iTunes Music Library (S).xml"));
}

TEST_CASE("Parse big file", "[parser]") {
    REQUIRE(parsing_succeeded("iTunes Music Library.xml"));
}