INCLUDE(CheckCXXSourceCompiles)

SET(AVX2_CODE "
  #include <immintrin.h>

  int main()
  {
    __m256i a = {0};
    a = _mm256_abs_epi16(a);
    __m256i x;
    _mm256_extract_epi64(x, 0); // we rely on this in our AVX2 code
    return 0;
  }
")

MACRO(CHECK_AVX2)
  #
  SET(type "AVX2")

  #
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    SET(flags "/arch:AVX2")
  else()
    SET(flags "-mavx2;-mfma")
  endif()

  SET(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})

    IF(NOT ${type}_FOUND)
      SET(CMAKE_REQUIRED_FLAGS ${flags})
      CHECK_CXX_SOURCE_COMPILES("${${type}_CODE}" HAS_${type})
      
      IF(HAS_${type})
        SET(${type}_FOUND TRUE CACHE BOOL "${type} support")
        SET(${type}_FLAGS ${flags} CACHE STRING "${type} flags")
      ENDIF()
    ENDIF()

  SET(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_SAVE})

  IF(NOT ${type}_FOUND)
    SET(${type}_FOUND FALSE CACHE BOOL "${lang} ${type} support")
    SET(${type}_FLAGS "" CACHE STRING "${lang} ${type} flags")
  ENDIF()

  MARK_AS_ADVANCED(${type}_FOUND ${type}_FLAGS)

ENDMACRO()

CHECK_AVX2()
