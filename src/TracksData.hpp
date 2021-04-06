#pragma once

#include "Tracks.hpp"
#include "RawTracksCollection.hpp"

struct TracksData : public MTBatcher<RawTracksCollection, FieldType::ITracksData>, public std::vector<FieldType::ITracksData> {
 public:
    TracksData(const Input&& rawTracks) : MTBatcher(this) {
        reserve(rawTracks.size());
        auto results = _processBatches(rawTracks);
        results.swap(*this);
    }
    
    TracksData(const TracksData&) = delete;
    void operator=(const TracksData&) = delete;

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