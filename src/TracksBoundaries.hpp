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

#include <string>

#include "helpers/AVX2Find.hpp"
#include "ITunesXMLLibrary.hpp"

struct TracksBoundaries : public std::string_view {
 public:
    TracksBoundaries(const ITunesXMLLibrary& lib) :
        std::string_view(_getBoundaries(lib)) {}
    ~TracksBoundaries() {}
    TracksBoundaries(const TracksBoundaries&) = delete;
    void operator=(const TracksBoundaries&) = delete;

    // uses string_view equal operator instead
    using std::string_view::operator=;

 private:
    static const std::string_view _getBoundaries(const ITunesXMLLibrary& lib) {
        auto searchSV = lib.asStringView();
        auto foundBegin = _findBeginTrackPos(searchSV);
        auto foundEnd   = _findEndTrackPos(searchSV);
        assert(foundEnd > foundBegin);

        // remove englobing <dict> from boundaries
        static constexpr const auto bDict = sizeof("\n\t<dict>");
        static constexpr const auto eDict = sizeof("</dict>\n\t");
        foundBegin += bDict;
        foundEnd -= eDict;

        //
        return { searchSV.data() + foundBegin, foundEnd - foundBegin };
    }

    static const std::size_t
        _findBeginTrackPos(const std::string_view& searchSV) {
        //
        static constexpr const auto _beginTracksPattern =
            std::string_view { "<key>Tracks</key>" };

        //
        auto foundBegin = avx2_find(searchSV, _beginTracksPattern);
        assert(foundBegin != std::string::npos);
        foundBegin += _beginTracksPattern.size();
        return foundBegin;
    }

    static const std::size_t
        _findEndTrackPos(const std::string_view& searchSV) {
        //
        static constexpr const auto _endTracksPattern =
            std::string_view { "<key>Playlists</key>" };

        //
        auto foundEnd = searchSV.rfind(_endTracksPattern);
        assert(foundEnd != std::string::npos);
        return foundEnd;
    }
};
