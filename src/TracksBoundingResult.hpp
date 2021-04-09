#pragma once

#include "helpers/AVX2Find.hpp"

#include "Tracks.hpp"
#include "RawTracksCollection.hpp"

struct TracksBoundingResult :   public MTBatcher<RawTracksCollection, FieldType::TrackFieldsBoundingResult>, 
                                public IPipeableSource<TracksBoundingResult>, 
                                public std::vector<FieldType::TrackFieldsBoundingResult> {
 public:
    TracksBoundingResult(const Input&& rawTracks) : MTBatcher(this), IPipeableSource(this) {
        //
        reserve(rawTracks.size());

        // process and fill
            // Single-threaded AVX2 version
            auto results = _processBatch(rawTracks, 0, 0);

            // Multi-threaded AVX2 version, slower than ST
            // auto results = _processBatches(rawTracks);
            // _fillSelfWithResults(results, boundaries.size());

        //
        results.swap(*this);
    }

    ~TracksBoundingResult() {}
    
    TracksBoundingResult(TracksBoundingResult&&) = default;
    TracksBoundingResult(const TracksBoundingResult&) = delete;
    void operator=(const TracksBoundingResult&) = delete;

    // fill defaulting values on missing disc number fields
    void fillDefaultingValues() {
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