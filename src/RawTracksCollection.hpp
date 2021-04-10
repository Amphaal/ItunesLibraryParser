#pragma once

#include "TracksBoundaries.hpp"

struct RawTracksCollection :    public IPipeableSource<RawTracksCollection>, 
                                public std::vector<std::string_view> {
 public:
    RawTracksCollection(TracksBoundaries &&boundaries) : IPipeableSource(this) { 
        // remove dict from boundaries
        static constexpr const auto bDict = sizeof("\n\t<dict>");
        static constexpr const auto eDict = sizeof("\n\t</dict>");
        boundaries = boundaries.substr(
            bDict,
            boundaries.size() - bDict - eDict
        );
        
        // process and fill
        _fill_ST_AVX2(boundaries);
    }

    ~RawTracksCollection() {}
    RawTracksCollection(RawTracksCollection&&) = default;
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

        while(true) {
            //
            startSearchingAt = input.data() + pos;
            remainingLengthToSearch = input.size() - pos;
            assert(remainingLengthToSearch > 0);
            assert(startSearchingAt < input.end());

            //
            found = avx2_find(
                startSearchingAt, remainingLengthToSearch,
                _endPattern.data(), _endPattern.size()
            );
            if(found == std::string::npos) {
                break;
            }

            //
            this->emplace_back(startSearchingAt, found);
            pos += found + _endPattern.size();
            auto i = true;
        }
    }
};
