#pragma once

#include "Async.hpp"
#include "TracksBoundaries.hpp"

struct RawTracksCollection :    public MTBatcher<TracksBoundaries, const char*>, 
                                public IPipeableSource<RawTracksCollection>, 
                                public std::vector<std::string_view> {
 public:
    RawTracksCollection(Input &&boundaries) : MTBatcher(this), IPipeableSource(this) { 
        // remove dict from boundaries
        static constexpr const auto bDict = sizeof("\n\t<dict>");
        static constexpr const auto eDict = sizeof("\n\t</dict>");
        boundaries = boundaries.substr(
            bDict,
            boundaries.size() - bDict - eDict
        );
        
        // process and fill

            // Multi-threaded AVX2 version, kinda slowish
            // auto results = _processBatches(boundaries);
            // _fillSelfWithResults(results, boundaries.size());

            // Single-threaded AVX2 version, faster !
            _fill_ST_AVX2(boundaries);
    }

    ~RawTracksCollection() {}

    RawTracksCollection(RawTracksCollection&&) = default;
    RawTracksCollection(const RawTracksCollection&) = delete;
    void operator=(const RawTracksCollection&) = delete;

 private:
    static constexpr const auto _endPattern = std::string_view { "</dict>" };
    static constexpr const std::size_t _avgTrackSize = 1500;
    
    void _fillSelfWithResults(const PackedOutput &toConvert, std::size_t boundariesSize) {
        //
        auto fromSize = toConvert.size();
        this->reserve(fromSize);

        //
        auto stopLimit = fromSize - 1;
        const char *pos;
        long size;
        std::size_t totalSize = 0;

        // all but last
        for(auto i = 0; i < stopLimit; i++) {
            pos = toConvert[i];
            size = toConvert[i + 1] - pos;
            totalSize += size;
            this->emplace_back(pos, size);
        }

        // finally, insert last
        this->emplace_back(toConvert.back(), boundariesSize - totalSize);
    }

    // collate a contiguous char* range of a master, preventing duplicates 
    virtual void _collateBatches(PackedOutput &toFill, const PackedOutput &toCollate) const final {
        //
        auto toCatchUp = toFill.back();

        //
        for(auto i = toCollate.cbegin(); i != toCollate.cend(); ++i) {
            if(*i <= toCatchUp) continue;    
            toFill.insert(toFill.end(), i, toCollate.end());
            return;
        }
    }

    const 

    void _fill_ST_AVX2(const Input & input) {
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
            found = avx2_naive_strstr64(
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

    virtual const PackedOutput _processBatch(std::reference_wrapper<const Input> inputRef, const std::size_t startAt, const std::size_t jobSize) const final {
        auto &input = inputRef.get();
        const auto inputSize = input.size();
        
        // approx avg. track dict size
        PackedOutput results;
        auto estimatedTrackCount = (inputSize - startAt) / _avgTrackSize;
        results.reserve(estimatedTrackCount);
        
        //
        std::size_t found;
        std::size_t pos = startAt;
        unsigned long long foundLength;
        unsigned long long alreadySearchedLength = 0;

        //
        while(true) {
            //
            found = avx2_naive_strstr64(input, _endPattern, pos);
            if(found == std::string::npos) {
                break;
            }

            //
            foundLength = found - pos;
            results.emplace_back(input.data() + pos);

            //
            if(jobSize) {
                alreadySearchedLength += foundLength;

                if(alreadySearchedLength > jobSize) {
                    break;
                }
            }
            
            //
            pos += foundLength + _endPattern.size();
        }

        //
        return results;
    }
};
