#pragma once

#include "Tracks.hpp"
#include "RawTracksCollection.hpp"

struct TracksData : public MTBatcher<RawTracksCollection, ITracksData>, public std::vector<ITracksData> {
 public:
    TracksData(const Input& rawTracks) : MTBatcher(this) {
        reserve(rawTracks.size());
        auto results = _processBatches(rawTracks);
        results.swap(*this);
    }
 
 private:
    virtual const PackedOutput _processBatch(const Input& input, const std::size_t startAt, const std::size_t jobSize) const final {
        //
        PackedOutput output;
        auto count = jobSize ? jobSize : input.size();
        output.reserve(count);

        //
        auto until = jobSize ? startAt + jobSize : input.size();
        for(auto i = startAt; i < until; ++i) {
            //
            const auto &source = input[i];
            auto &dest = output.emplace_back();
            auto &result = dest.trackFields;
            auto &missing = dest.missingFields;

            //
            std::size_t pos = 0;

            //
            FieldType::TrackID      .scanFill(source, pos, result[0], missing[0]);
            FieldType::TrackNumber  .scanFill(source, pos, result[1], missing[1]);
            FieldType::Year         .scanFill(source, pos, result[2], missing[2]);
            FieldType::DateAdded    .scanFill(source, pos, result[3], missing[3]);
            FieldType::Name         .scanFill(source, pos, result[4], missing[4]);
            FieldType::AlbumArtist  .scanFill(source, pos, result[5], missing[5]);
            FieldType::Album        .scanFill(source, pos, result[6], missing[6]);
            FieldType::Genre        .scanFill(source, pos, result[7], missing[7]);
        }

        //
        return output;
    };
};