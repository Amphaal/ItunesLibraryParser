#pragma once

#include "ITunesXMLLibrary.hpp"

struct TracksBoundaries : public std::string_view { 
    using std::string_view::operator=;
    TracksBoundaries(const ITunesXMLLibrary& lib) : std::string_view(_getBoundaries(lib)) {}

 private:
    static const std::string_view _getBoundaries(const ITunesXMLLibrary& lib) {
        auto searchSV = lib.asStringView();
        auto foundBegin = _findBeginTrackPos(searchSV);
        auto foundEnd   = _findEndTrackPos(searchSV);
        assert(foundEnd > foundBegin);

        //
        return { lib.ptr + foundBegin, foundEnd - foundBegin };
    }

    static const std::size_t _findBeginTrackPos(const std::string_view& searchSV) {
        static constexpr const char _beginTracksPattern[] = "<key>Tracks</key>";
        auto foundBegin = searchSV.find(_beginTracksPattern);
        assert(foundBegin != searchSV.npos);
        foundBegin += sizeof(_beginTracksPattern);
        return foundBegin;
    }

    static const std::size_t _findEndTrackPos(const std::string_view& searchSV) {
        static constexpr const char _endTracksPattern[] = "<key>Playlists</key>";
        auto foundEnd = searchSV.rfind(_endTracksPattern);
        assert(foundEnd != searchSV.npos);
        return foundEnd;
    }
};
