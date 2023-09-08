#include <string>
#include <stdint.h>

#include <arm_neon.h>

# define USE_SIMPLE_MEMCMP // for fixed-memcmp.cpp

#define MAYBE_UNUSED inline __attribute__((unused))
# define FORCE_INLINE __attribute__((always_inline)) inline

namespace {

    MAYBE_UNUSED
    bool always_true(const char*, const char*) {
        return true;
    }

    MAYBE_UNUSED
    bool memcmp1(const char* a, const char* b) {
        return a[0] == b[0];
    }

    MAYBE_UNUSED
    bool memcmp2(const char* a, const char* b) {
        const uint16_t A = *reinterpret_cast<const uint16_t*>(a);
        const uint16_t B = *reinterpret_cast<const uint16_t*>(b);
        return A == B;
    }

    MAYBE_UNUSED
    bool memcmp3(const char* a, const char* b) {

#ifdef USE_SIMPLE_MEMCMP
        return memcmp2(a, b) && memcmp1(a + 2, b + 2);
#else
        const uint32_t A = *reinterpret_cast<const uint32_t*>(a);
        const uint32_t B = *reinterpret_cast<const uint32_t*>(b);
        return (A & 0x00ffffff) == (B & 0x00ffffff);
#endif
    }

    MAYBE_UNUSED
    bool memcmp4(const char* a, const char* b) {

        const uint32_t A = *reinterpret_cast<const uint32_t*>(a);
        const uint32_t B = *reinterpret_cast<const uint32_t*>(b);
        return A == B;
    }

    MAYBE_UNUSED
    bool memcmp5(const char* a, const char* b) {

#ifdef USE_SIMPLE_MEMCMP
        return memcmp4(a, b) && memcmp1(a + 4, b + 4);
#else
        const uint64_t A = *reinterpret_cast<const uint64_t*>(a);
        const uint64_t B = *reinterpret_cast<const uint64_t*>(b);
        return ((A ^ B) & 0x000000fffffffffflu) == 0;
#endif
    }

    MAYBE_UNUSED
    bool memcmp6(const char* a, const char* b) {

#ifdef USE_SIMPLE_MEMCMP
        return memcmp4(a, b) && memcmp2(a + 4, b + 4);
#else
        const uint64_t A = *reinterpret_cast<const uint64_t*>(a);
        const uint64_t B = *reinterpret_cast<const uint64_t*>(b);
        return ((A ^ B) & 0x0000fffffffffffflu) == 0;
#endif
    }

    MAYBE_UNUSED
    bool memcmp7(const char* a, const char* b) {

#ifdef USE_SIMPLE_MEMCMP 
        return memcmp4(a, b) && memcmp3(a + 4, b + 4);
#else
        const uint64_t A = *reinterpret_cast<const uint64_t*>(a);
        const uint64_t B = *reinterpret_cast<const uint64_t*>(b);
        return ((A ^ B) & 0x00fffffffffffffflu) == 0;
#endif
    }

    MAYBE_UNUSED
    bool memcmp8(const char* a, const char* b) {

        const uint64_t A = *reinterpret_cast<const uint64_t*>(a);
        const uint64_t B = *reinterpret_cast<const uint64_t*>(b);
        return A == B;
    }

    MAYBE_UNUSED
    bool memcmp9(const char* a, const char* b) {

        const uint64_t A = *reinterpret_cast<const uint64_t*>(a);
        const uint64_t B = *reinterpret_cast<const uint64_t*>(b);
        return (A == B) & (a[8] == b[8]);
    }

    MAYBE_UNUSED
    bool memcmp10(const char* a, const char* b) {

        const uint64_t Aq = *reinterpret_cast<const uint64_t*>(a);
        const uint64_t Bq = *reinterpret_cast<const uint64_t*>(b);
        const uint16_t Aw = *reinterpret_cast<const uint16_t*>(a + 8);
        const uint16_t Bw = *reinterpret_cast<const uint16_t*>(b + 8);
        return (Aq == Bq) & (Aw == Bw);
    }

    MAYBE_UNUSED
    bool memcmp11(const char* a, const char* b) {

#ifdef USE_SIMPLE_MEMCMP
        return memcmp8(a, b) && memcmp3(a + 8, b + 8);
#else
        const uint64_t Aq = *reinterpret_cast<const uint64_t*>(a);
        const uint64_t Bq = *reinterpret_cast<const uint64_t*>(b);
        const uint32_t Ad = *reinterpret_cast<const uint32_t*>(a + 8);
        const uint32_t Bd = *reinterpret_cast<const uint32_t*>(b + 8);
        return (Aq == Bq) & ((Ad & 0x00ffffff) == (Bd & 0x00ffffff));
#endif
    }

    MAYBE_UNUSED
    bool memcmp12(const char* a, const char* b) {

        const uint64_t Aq = *reinterpret_cast<const uint64_t*>(a);
        const uint64_t Bq = *reinterpret_cast<const uint64_t*>(b);
        const uint32_t Ad = *reinterpret_cast<const uint32_t*>(a + 8);
        const uint32_t Bd = *reinterpret_cast<const uint32_t*>(b + 8);
        return (Aq == Bq) & (Ad == Bd);
    }

}

//
//
//

size_t FORCE_INLINE neon_strstr_anysize(const char* s, size_t n, const char* needle, size_t k) {

    assert(k > 0);
    assert(n > 0);

    const uint8x16_t first = vdupq_n_u8(needle[0]);
    const uint8x16_t last  = vdupq_n_u8(needle[k - 1]);
    const uint8x8_t  half  = vdup_n_u8(0x0f);

    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(s);

    union {
        uint8_t  tmp[8];
        uint32_t word[2];
    };

    for (size_t i = 0; i < n; i += 16) {

        const uint8x16_t block_first = vld1q_u8(ptr + i);
        const uint8x16_t block_last  = vld1q_u8(ptr + i + k - 1);

        const uint8x16_t eq_first = vceqq_u8(first, block_first);
        const uint8x16_t eq_last  = vceqq_u8(last, block_last);
        const uint8x16_t pred_16  = vandq_u8(eq_first, eq_last);
        const uint8x8_t pred_8    = vbsl_u8(half, vget_low_u8(pred_16), vget_high_u8(pred_16));

        vst1_u8(tmp, pred_8);

        if ((word[0] | word[1]) == 0) {
            continue;
        }

#if 0
        for (int j=0; j < 8; j++) {
            if ((tmp[j] & 0x0f) && (memcmp(s + i + j + 1, needle + 1, k - 2) == 0)) {
                return i + j;
            }
        }

        for (int j=0; j < 8; j++) {
            if ((tmp[j] & 0xf0) && (memcmp(s + i + j + 1 + 8, needle + 1, k - 2) == 0)) {
                return i + j + 8;
            }
        }
#else
        // the above loops unrolled
        uint32_t v;

#define RETURN_IF_EQ(MASK, SHIFT) \
        if ((v & MASK) && memcmp(s + i + SHIFT + 1, needle + 1, k - 2) == 0) { \
            return i + SHIFT; \
        }

#define COMPARE(MASK, WORD_IDX, SHIFT) \
        v = word[WORD_IDX];      \
        RETURN_IF_EQ(MASK, SHIFT + 0); \
        v >>= 8; \
        RETURN_IF_EQ(MASK, SHIFT + 1); \
        v >>= 8; \
        RETURN_IF_EQ(MASK, SHIFT + 2); \
        v >>= 8; \
        RETURN_IF_EQ(MASK, SHIFT + 3);

        COMPARE(0x0f, 0,  0);
        COMPARE(0x0f, 1,  4);
        COMPARE(0xf0, 0,  8);
        COMPARE(0xf0, 1, 12);

#undef RETURN_IF_EQ
#undef COMPARE

#endif
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

template <size_t k, typename MEMCMP>
size_t FORCE_INLINE neon_strstr_memcmp(const char* s, size_t n, const char* needle, MEMCMP memcmp_fun) {

    assert(k > 0);
    assert(n > 0);

    const uint8x16_t first = vdupq_n_u8(needle[0]);
    const uint8x16_t last  = vdupq_n_u8(needle[k - 1]);
    const uint8x8_t  half  = vdup_n_u8(0x0f);

    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(s);

    union {
        uint8_t  tmp[8];
        uint32_t word[2];
    };

    for (size_t i = 0; i < n; i += 16) {

        const uint8x16_t block_first = vld1q_u8(ptr + i);
        const uint8x16_t block_last  = vld1q_u8(ptr + i + k - 1);

        const uint8x16_t eq_first = vceqq_u8(first, block_first);
        const uint8x16_t eq_last  = vceqq_u8(last, block_last);
        const uint8x16_t pred_16  = vandq_u8(eq_first, eq_last);
        const uint8x8_t pred_8    = vbsl_u8(half, vget_low_u8(pred_16), vget_high_u8(pred_16));

        vst1_u8(tmp, pred_8);

        if ((word[0] | word[1]) == 0) {
            continue;
        }

#if 0
        for (int j=0; j < 8; j++) {
            if ((tmp[j] & 0x0f) && memcmp_fun(s + i + j + 1, needle + 1)) {
                return i + j;
            }
        }

        for (int j=0; j < 8; j++) {
            if ((tmp[j] & 0xf0) && memcmp_fun(s + i + j + 1 + 8, needle + 1)) {
                return i + j + 8;
            }
        }
#else
        // the above loops unrolled
        uint32_t v;

#define RETURN_IF_EQ(MASK, SHIFT) \
        if ((v & MASK) && memcmp_fun(s + i + SHIFT + 1, needle + 1)) { \
            return i + SHIFT; \
        }

#define COMPARE(MASK, WORD_IDX, SHIFT) \
        v = word[WORD_IDX];      \
        RETURN_IF_EQ(MASK, SHIFT + 0); \
        v >>= 8; \
        RETURN_IF_EQ(MASK, SHIFT + 1); \
        v >>= 8; \
        RETURN_IF_EQ(MASK, SHIFT + 2); \
        v >>= 8; \
        RETURN_IF_EQ(MASK, SHIFT + 3);

        COMPARE(0x0f, 0,  0);
        COMPARE(0x0f, 1,  4);
        COMPARE(0xf0, 0,  8);
        COMPARE(0xf0, 1, 12);

#undef RETURN_IF_EQ
#undef COMPARE

#endif
    }

    return std::string::npos;
}

//
//
//

// ------------------------------------------------------------------------

size_t simd_find(const char* s, size_t n, const char* needle, size_t k) {

    size_t result = std::string::npos;

    if (n < k) {
        return result;
    }

	switch (k) {
		case 0:
			return 0;

		case 1: {
            const char* res = reinterpret_cast<const char*>(strchr(s, needle[0]));

			return (res != nullptr) ? res - s : std::string::npos;
            }

        case 2:
            result = neon_strstr_memcmp<2>(s, n, needle, always_true);
            break;

        case 3:
            result = neon_strstr_memcmp<3>(s, n, needle, memcmp1);
            break;

        case 4:
            result = neon_strstr_memcmp<4>(s, n, needle, memcmp2);
            break;

        case 5:
            result = neon_strstr_memcmp<5>(s, n, needle, memcmp4);
            break;

        case 6:
            result = neon_strstr_memcmp<6>(s, n, needle, memcmp4);
            break;

        case 7:
            result = neon_strstr_memcmp<7>(s, n, needle, memcmp5);
            break;

        case 8:
            result = neon_strstr_memcmp<8>(s, n, needle, memcmp6);
            break;

        case 9:
            result = neon_strstr_memcmp<9>(s, n, needle, memcmp8);
            break;

        case 10:
            result = neon_strstr_memcmp<10>(s, n, needle, memcmp8);
            break;

        case 11:
            result = neon_strstr_memcmp<11>(s, n, needle, memcmp9);
            break;

        case 12:
            result = neon_strstr_memcmp<12>(s, n, needle, memcmp10);
            break;

		default:
			result = neon_strstr_anysize(s, n, needle, k);
            break;
    }

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// ------------------------------------------------------------------------

size_t simd_find(const std::string_view& s, const std::string_view& needle) {

    return simd_find(s.data(), s.size(), needle.data(), needle.size());
}


size_t simd_find(const std::string_view& s,
                const std::string_view& needle,
                size_t pos) {
    //
    if (pos >= s.size()) return std::string::npos;

    //
    auto searchFrom = s.data() + pos;
    auto found = simd_find(searchFrom, s.size() - pos,
                            needle.data(),
                            needle.size());

    //
    if (found != std::string::npos)
        return found + pos;

    //
    return found;
}
