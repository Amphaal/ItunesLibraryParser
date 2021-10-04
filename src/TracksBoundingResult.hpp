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

#include "helpers/AVX2Find.hpp"

#include "config/TracksScan.hpp"
#include "RawTracksCollection.hpp"

struct TracksBoundingResult
    : public std::vector<FieldType::TrackFieldsBoundingResult> {

 public:
    TracksBoundingResult(const RawTracksCollection&& rawTracks) {
        // process and fill
        _fill_ST_AVX2(rawTracks);
        _fillDefaultingValues();
    }

    ~TracksBoundingResult() {}
    TracksBoundingResult(const TracksBoundingResult&) = delete;
    void operator=(const TracksBoundingResult&) = delete;

 private:
    void  _fill_ST_AVX2(const RawTracksCollection& rawTracks) {
        //
        const auto tracksC = rawTracks.size();
        reserve(tracksC);

        //
        for (const auto &rawTrack : rawTracks) {
            //
            auto &dest = this->emplace_back();

            //
            std::size_t pos = 0;

            //
            for (auto i = 0; i < FieldType::orderedScans.size(); ++i) {
                const auto &fieldType = FieldType::orderedScans[i];
                fieldType->scanFill(rawTrack, pos, dest);
            }
        }
    }

    // fill defaulting values on missing disc number fields
    void _fillDefaultingValues() {
        //
        constexpr const auto numP = std::string_view { "1" };
        constexpr const auto discNumberIndex = FieldType::DiscNumber.index;

        //
        for (auto &trackData : *this) {
            //
            auto &target = trackData.missingFields[discNumberIndex];
            if (!target) continue;

            //
            trackData.trackFields[discNumberIndex] = numP;
            target = false;
        }
    }
};
