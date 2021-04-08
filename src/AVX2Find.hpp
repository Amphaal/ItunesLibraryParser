//
// Improvements are noticeable on release
//

#pragma once

#include <assert.h>
#include <immintrin.h>
#include <stdint.h>
#include <string.h>

#include <string>

# define FORCE_INLINE __attribute__((always_inline)) inline

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

} // namespace bits


size_t FORCE_INLINE avx2_naive_strstr_anysize64(const char* s, size_t n, const char* needle, size_t k) {

    assert(k > 0);
    assert(n > 0);
    const __m256i first = _mm256_set1_epi8(needle[0]);
    const __m256i last  = _mm256_set1_epi8(needle[k - 1]);
    for (size_t i = 0; i < n; i += 64) {

        const __m256i block_first1 = _mm256_loadu_si256((const __m256i*)(s + i));
        const __m256i block_last1  = _mm256_loadu_si256((const __m256i*)(s + i + k - 1));

        const __m256i block_first2 = _mm256_loadu_si256((const __m256i*)(s + i + 32));
        const __m256i block_last2  = _mm256_loadu_si256((const __m256i*)(s + i + k - 1 + 32));

        const __m256i eq_first1 = _mm256_cmpeq_epi8(first, block_first1);
        const __m256i eq_last1  = _mm256_cmpeq_epi8(last, block_last1);

        const __m256i eq_first2 = _mm256_cmpeq_epi8(first, block_first2);
        const __m256i eq_last2  = _mm256_cmpeq_epi8(last, block_last2);

        const uint32_t mask1 = _mm256_movemask_epi8(_mm256_and_si256(eq_first1, eq_last1));
        const uint32_t mask2 = _mm256_movemask_epi8(_mm256_and_si256(eq_first2, eq_last2));
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

size_t avx2_naive_strstr64(const char* s, size_t n, const char* needle, size_t k) {

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

size_t avx2_naive_strstr64(const std::string_view& s, const std::string_view& needle) {
    return avx2_naive_strstr64(s.data(), s.size(), needle.data(), needle.size());
}

size_t avx2_naive_strstr64(const std::string_view& s, const std::string_view& needle, size_t pos) {
    return avx2_naive_strstr64(s.data() + pos, s.size(), needle.data(), needle.size());
}
