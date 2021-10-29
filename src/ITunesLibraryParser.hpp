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

#pragma once

#include <utility>

#include "Parser.hpp"

class ITunesLibraryParser {
 public:
    ITunesLibraryParser(const char * xmlFilePath,
                        const char * outputJSONFilePath,
                        const char * warningJSONFilePath) noexcept :
    _xmlFilePath(xmlFilePath),
    _outputJSONFilePath(outputJSONFilePath),
    _warningJSONFilePath(warningJSONFilePath) {}

    void produceOutputs() const {
        //
        auto [OKTracks, missingFieldsTracks] = getStoragedResults();

        // if has missing fields tracks
        if(missingFieldsTracks.size()) {
            MissingFieldsJSONParser { std::move(missingFieldsTracks) }
                .copyToFile(_warningJSONFilePath);
        }

        //
        SuccessfulJSONParser { std::move(OKTracks) }
            .copyToFile(_outputJSONFilePath);
    }

    PackedTracks getStoragedResults() const {
        const auto inMemoryFile = ITunesXMLLibrary { _xmlFilePath };

        //
        return {
            RawTracksCollection {
                inMemoryFile
            }
        };
    }

 private:
    const char * _xmlFilePath;
    const char * _outputJSONFilePath;
    const char * _warningJSONFilePath;
};
