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

#pragma once

#include <cassert>
#include <string_view>
#include <vector>
#include <array>
#include <string>

namespace FieldType {

template<size_t N>
struct StringLiteral {
 public:
    constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    char value[N];
};

template<int8_t SizeVal>
struct ITrackFieldsBoundingResult {
    static constexpr const auto Size = SizeVal;
    std::array<std::string_view, SizeVal>  trackFields;
    std::array<bool, SizeVal>              missingFields;
};

// determine number of fields to scan
typedef ITrackFieldsBoundingResult<9> TrackFieldsBoundingResult;

template<auto S = FieldType::TrackFieldsBoundingResult::Size>
struct IOutputContainer :
    public std::vector<std::array<std::string_view, S>> {};

using OutputContainer = IOutputContainer<>;

struct IPackedTracks {
    OutputContainer OKTracks;
    OutputContainer missingFieldsTracks;
};

struct IScanner {
 public:
    virtual void scanFill(
        const std::string_view &source,
        std::size_t &pos,
        TrackFieldsBoundingResult &result) const = 0;
    virtual constexpr const std::string_view& fieldName() const = 0;
};

template<
    unsigned int IndexT,
    StringLiteral FieldName,
    StringLiteral LBegin,
    StringLiteral LEnd>
struct FieldTypeStruct : public IScanner {
    // make sure any FieldTypeStruct respects boundaries
    static_assert(IndexT < TrackFieldsBoundingResult::Size);

 public:
    void scanFill(
        const std::string_view &source,
        std::size_t &pos,
        TrackFieldsBoundingResult &result) const final {
        //
        auto &trackField = result.trackFields[index];
        auto &missingField = result.missingFields[index];

        //
        auto foundBegin = avx2_find(source, _beginFP, pos);
        if (foundBegin == std::string::npos) {
            missingField = true;
            return;
        }

        //
        foundBegin += _beginFP.size();
        pos = foundBegin;

        //
        auto foundEnd = avx2_find(source, _endFP, pos);
        if (foundEnd == std::string::npos) {
            missingField = true;
            return;
        }

        //
        auto dataFieldLength = foundEnd - foundBegin;
        pos += dataFieldLength + _endFP.size();

        //
        trackField = std::string_view {
            source.data() + foundBegin, // use data() instead of begin() to avoid implicit casting issues
            dataFieldLength
        };
    }

    constexpr const std::string_view& fieldName() const final {
        return _fieldName;
    }

    static constexpr const auto _fieldName =
        std::string_view { FieldName.value, sizeof(FieldName.value) - 1 };

    static constexpr const auto index = IndexT;

 private:
    static constexpr const auto _beginFP =
        std::string_view { LBegin.value, sizeof(LBegin.value) - 1 };
    static constexpr const auto _endFP =
        std::string_view { LEnd.value, sizeof(LEnd.value) - 1 };
};

using OrderedScansT =
    const std::array<const IScanner*, TrackFieldsBoundingResult::Size>;

}  // namespace FieldType
