// ItunesLibraryParser
// Allows JSON parsing of XML Itunes Library file
// Copyright (C) 2021 Guillaume Vara <guillaume.vara@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Graphical resources without explicit references to a
// different license and copyright still refer to this GPL.

//
// Improvements are noticeable on release
//

#pragma once

#include <cassert>
#include <immintrin.h>
#include <stdint.h>
#include <string.h>

#include <string>

#ifndef __APPLE__
# define FORCE_INLINE __attribute__((always_inline)) inline
#else
# define FORCE_INLINE 
#endif

namespace bits {

    template <typename T>
    T clear_leftmost_set(const T value) {
        assert(value != 0);

        return value & (value - 1);
    }

    template <typename T>
    unsigned get_first_bit_set(const T value) {
        assert(value != 0);

        return __builtin_ctz(value);
    }

    template <>
    unsigned get_first_bit_set<uint64_t>(const uint64_t value) {
        assert(value != 0);

        return __builtin_ctzl(value);
    }

}  // namespace bits


const __m256i avx2_sc_load256(const char* val) {
    return _mm256_loadu_si256((const __m256i*)(val));
}


const uint32_t avx2_sc_maskgen(
    const __m256i eq_first,
    const __m256i eq_last) {
    return _mm256_movemask_epi8(_mm256_and_si256(eq_first, eq_last));
}


size_t FORCE_INLINE avx2_naive_strstr_anysize64(const char* s,
                                                size_t n,
                                                const char* needle,
                                                size_t k) {
    assert(k > 0);
    assert(n > 0);
    const auto first = _mm256_set1_epi8(needle[0]);
    const auto last  = _mm256_set1_epi8(needle[k - 1]);
    for (size_t i = 0; i < n; i += 64) {
        const auto block_first1 = avx2_sc_load256(s + i);
        const auto block_last1  = avx2_sc_load256(s + i + k - 1);

        const auto block_first2 = avx2_sc_load256(s + i + 32);
        const auto block_last2  = avx2_sc_load256(s + i + k - 1 + 32);

        const __m256i eq_first1 = _mm256_cmpeq_epi8(first, block_first1);
        const __m256i eq_last1  = _mm256_cmpeq_epi8(last, block_last1);

        const __m256i eq_first2 = _mm256_cmpeq_epi8(first, block_first2);
        const __m256i eq_last2  = _mm256_cmpeq_epi8(last, block_last2);

        const auto mask1 = avx2_sc_maskgen(eq_first1, eq_last1);
        const auto mask2 = avx2_sc_maskgen(eq_first2, eq_last2);
        uint64_t mask = mask1 | ((uint64_t)mask2 << 32);

        while (mask != 0) {
            const int bitpos = __builtin_ctzll(mask);
            if (memcmp(s + i + bitpos + 1, needle + 1, k - 2) == 0) {
                return i + bitpos;
            }
            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}


// ------------------------------------------------------------------------

size_t avx2_find(const char* s, size_t n, const char* needle, size_t k) {
    size_t result = std::string::npos;

    if (n < k) {
        return result;
    }

    result = avx2_naive_strstr_anysize64(s, n, needle, k);

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// ------------------------------------------------------------------------

size_t avx2_find(const std::string_view& s, const std::string_view& needle) {
    return avx2_find(s.data(), s.size(), needle.data(), needle.size());
}

size_t avx2_find(const std::string_view& s,
                const std::string_view& needle,
                size_t pos) {
    //
    if (pos >= s.size()) return std::string::npos;

    //
    auto searchFrom = s.data() + pos;
    auto found = avx2_find(searchFrom, s.size() - pos,
                            needle.data(),
                            needle.size());

    //
    if (found != std::string::npos)
        return found + pos;

    //
    return found;
}
