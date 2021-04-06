#pragma once

#include "Tracks.hpp"
#include "RawTracksCollection.hpp"

struct TracksBoundingResult : public MTBatcher<RawTracksCollection, FieldType::TrackFieldsBoundingResult>, public IPipeableSource<TracksBoundingResult>, public std::vector<FieldType::TrackFieldsBoundingResult> {
 public:
    TracksBoundingResult(const Input&& rawTracks) : MTBatcher(this), IPipeableSource(this) {
        reserve(rawTracks.size());
        auto results = _processBatches(rawTracks);
        results.swap(*this);
    }

    ~TracksBoundingResult() {}
    
    TracksBoundingResult(TracksBoundingResult&&) = default;
    TracksBoundingResult(const TracksBoundingResult&) = delete;
    void operator=(const TracksBoundingResult&) = delete;

    void fillDefaultingValuesOnMissingFields() {
        //
        constexpr auto numP = std::string_view {"1"};
        constexpr auto discNumberIndex = FieldType::DiscNumber.index;
        
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

 private:
    virtual const PackedOutput _processBatch(std::reference_wrapper<const Input> inputRef, const std::size_t startAt, const std::size_t jobSize) const final {
        //
        auto &input = inputRef.get();

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

            //
            std::size_t pos = 0;

            //
            for(auto i = 0; i < FieldType::orderedScans.size(); ++i) {
                auto &fieldType = FieldType::orderedScans[i];
                fieldType->scanFill(source, pos, dest);
            }
        }

        //
        return output;
    };
};