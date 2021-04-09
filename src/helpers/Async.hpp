#pragma once

#include <assert.h>

#include <future>
#include <vector>

template<typename InputT, typename BatchedOutputT>
class MTBatcher {
 public:
    using PackedOutput = std::vector<BatchedOutputT>;
    using Input = InputT;
    
    MTBatcher(const MTBatcher* target) : _target(target), _howManyThreads(std::thread::hardware_concurrency()) {
        assert(_target);
        assert(_howManyThreads > 1);
    }

 protected:
    const unsigned int _howManyThreads = 0;
    const MTBatcher* _target;

    const PackedOutput _processBatches(const Input &input) {
        //
        const auto jobDefaultSize = input.size() / _howManyThreads;
        size_t jobPosStart = 0;

        //
        std::future<const PackedOutput> futures[_howManyThreads];

        // create batched jobs
        auto last = _howManyThreads - 1;
        for(auto i = 0; i < last; ++i) {
            futures[i] = std::async(std::launch::async, &_processBatch, _target, std::cref(input), jobPosStart, jobDefaultSize);
            jobPosStart += jobDefaultSize;
        }
        
        // last batched job take remaining
        futures[last] = std::async(std::launch::async, &_processBatch, _target, std::cref(input), jobPosStart, 0);
        
        // wait for first results
        auto results = futures[0].get();

        // then collate
        for(auto i = 1; i < _howManyThreads; ++i) {
            _collateBatches(results, futures[i].get());
        }

        //
        return results;
    }

    // default collation method
    virtual void _collateBatches(PackedOutput &toFill, const PackedOutput &toCollate) const {
        toFill.insert(toFill.end(), toCollate.begin(), toCollate.end());
    };

    virtual const PackedOutput _processBatch(std::reference_wrapper<const Input> inputRef, const std::size_t startAt, const std::size_t jobSize) const = 0;
};
