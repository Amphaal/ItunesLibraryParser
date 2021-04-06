#pragma once

#include "TracksBoundingResult.hpp"

class PackedTracks : public IPipeableSource<PackedTracks>, public FieldType::IPackedTracks<> {
 public:
    PackedTracks(TracksBoundingResult&& orderedTracks) : IPipeableSource(this) {
        // for(const auto &trackData : orderedTracks) {
        //     trackData.
        // }
    }

    ~PackedTracks() {}
    
    PackedTracks(PackedTracks&&) = default;
    PackedTracks(const PackedTracks&) = delete;
    void operator=(const PackedTracks&) = delete;
};
