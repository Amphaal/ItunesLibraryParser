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

#include <vector>
#include <string>

#include "TracksBoundaries.hpp"

struct RawTracksCollection : public std::vector<std::string_view> {
 public:
    RawTracksCollection(const TracksBoundaries &&boundaries) {
        // process and fill
        _fill_ST_AVX2(boundaries);
    }

    ~RawTracksCollection() {}
    RawTracksCollection(const RawTracksCollection&) = delete;
    void operator=(const RawTracksCollection&) = delete;

 private:
    static constexpr const auto _endPattern = std::string_view { "</dict>" };
    static constexpr const std::size_t _avgTrackSize = 1500;

    // Single-threaded AVX2
    void _fill_ST_AVX2(const TracksBoundaries & input) {
        // approx avg. track dict size
        const auto estimatedTrackCount = input.size() / _avgTrackSize;
        this->reserve(estimatedTrackCount);

        std::size_t found;
        std::size_t remainingLengthToSearch;
        std::size_t pos = 0;
        const char * startSearchingAt;

        while (true) {
            //
            startSearchingAt = input.data() + pos;
            remainingLengthToSearch = input.size() - pos;
            assert(remainingLengthToSearch > 0);
            assert(input.end() > startSearchingAt);

            //
            found = avx2_find(
                startSearchingAt,
                remainingLengthToSearch,
                _endPattern.data(),
                _endPattern.size());

            //
            if (found == std::string::npos) {
                break;
            }

            //
            this->emplace_back(startSearchingAt, found);
            pos += found + _endPattern.size();
            auto i = true;
        }
    }
};
