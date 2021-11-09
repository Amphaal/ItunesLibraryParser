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

bool parsing_succeeded(const char * fileName) {
    const auto parser = ITunesLibraryParser {
        fileName,
        "output.json",
        "warnings.json"
    };

    parser.produceOutputs();

    return true;
}

TEST_CASE("Failing parsing on bad file name", "[parser]") {
    REQUIRE_THROWS(parsing_succeeded("iTunes Music Library (Ssss).xml"));
}

TEST_CASE("Parse small file", "[parser]") {
    const auto targetFile = "iTunes Music Library (S).xml";

    REQUIRE(parsing_succeeded(targetFile));

    BENCHMARK("Parsing") {
        return parsing_succeeded(targetFile);
    };
}

TEST_CASE("Parse big file", "[parser]") {
    const auto targetFile = "iTunes Music Library.xml";

    REQUIRE(parsing_succeeded(targetFile));

    BENCHMARK("Parsing") {
        return parsing_succeeded(targetFile);
    };
}
