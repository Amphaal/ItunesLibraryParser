#pragma once

#include "helpers/AVX2Find.hpp"

#include "Tracks.hpp"
#include "RawTracksCollection.hpp"

struct TracksBoundingResult : public std::vector<FieldType::TrackFieldsBoundingResult> {
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
        for(const auto &rawTrack : rawTracks) {
            //
            auto &dest = this->emplace_back();

            //
            std::size_t pos = 0;

            //
            for(auto i = 0; i < FieldType::orderedScans.size(); ++i) {
                const auto &fieldType = FieldType::orderedScans[i];
                fieldType->scanFill(rawTrack, pos, dest);
            }
        }
    };

    // fill defaulting values on missing disc number fields
    void _fillDefaultingValues() {
        //
        constexpr const auto numP = std::string_view { "1" };
        constexpr const auto discNumberIndex = FieldType::DiscNumber.index;
        
        //
        for(auto &trackData : *this) {
            //
            auto &target = trackData.missingFields[discNumberIndex];
            if(!target) continue;

            //
            trackData.trackFields[discNumberIndex] = numP;
            target = false;
        }
    }
};