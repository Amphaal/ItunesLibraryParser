#pragma once

#include "helpers/AVX2Find.hpp"
#include "ITunesXMLLibrary.hpp"

struct TracksBoundaries : public IPipeable<TracksBoundaries>, 
                          public std::string_view { 
 public:
    TracksBoundaries(const ITunesXMLLibrary& lib) : IPipeable(this), std::string_view(_getBoundaries(lib)) {}

    ~TracksBoundaries() {}
    TracksBoundaries(TracksBoundaries&&) = default;
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

        //
        return { searchSV.data() + foundBegin, foundEnd - foundBegin };
    }

    static const std::size_t _findBeginTrackPos(const std::string_view& searchSV) {
        //
        static constexpr const auto _beginTracksPattern = std::string_view { "<key>Tracks</key>" };

        //
        auto foundBegin = avx2_find(searchSV, _beginTracksPattern);
        assert(foundBegin != std::string::npos);
        foundBegin += _beginTracksPattern.size();
        return foundBegin;
    }

    static const std::size_t _findEndTrackPos(const std::string_view& searchSV) {
        //
        static constexpr const auto _endTracksPattern = std::string_view { "<key>Playlists</key>" };

        //
        auto foundEnd = searchSV.rfind(_endTracksPattern);
        assert(foundEnd != std::string::npos);
        return foundEnd;
    }
};
