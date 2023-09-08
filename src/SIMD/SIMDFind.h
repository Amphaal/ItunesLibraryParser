#include <string>
#include <stdint.h>

size_t simd_find(const char* s, size_t n, const char* needle, size_t k);
size_t simd_find(const std::string_view& s, const std::string_view& needle);
size_t simd_find(const std::string_view& s, const std::string_view& needle, size_t pos);
