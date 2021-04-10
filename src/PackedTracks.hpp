#pragma once

#include "TracksBoundingResult.hpp"

class PackedTracks :    public IPipeableSource<PackedTracks>, 
                        public FieldType::IPackedTracks {
 public:
    PackedTracks(const TracksBoundingResult& orderedTracks) : IPipeableSource(this) {
        for(const auto &trackData : orderedTracks) {
            //
            bool hasMissing = false;
            for(const auto &isMissing : trackData.missingFields) {
                if(isMissing) {
                    hasMissing = isMissing;
                    break;
                }
            }

            //
            if(hasMissing) {
                missingFieldsTracks.emplace_back(trackData.trackFields);
            } else {
                OKTracks.emplace_back(trackData.trackFields);
            }
        }
    }

    ~PackedTracks() {}
    PackedTracks(PackedTracks&&) = default;
    PackedTracks(const PackedTracks&) = delete;
    void operator=(const PackedTracks&) = delete;

    std::size_t allTracksCount() const {
        return OKTracks.size() + missingFieldsTracks.size();
    }
};
