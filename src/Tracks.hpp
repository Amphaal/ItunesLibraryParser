#pragma once

#include <assert.h>

#include <string_view>
#include <vector>
#include <array>

namespace FieldType {

template<size_t N>
struct StringLiteral {
 public:
    constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    
    char value[N];
};

template<unsigned short SizeVal>
struct ITrackFieldsBoundingResult {
    static constexpr auto Size = SizeVal;
    std::array<std::string_view, SizeVal>  trackFields;
    std::array<bool, SizeVal>              missingFields;
};

// determine number of fields to scan
typedef ITrackFieldsBoundingResult<9> TrackFieldsBoundingResult;

template<auto S = FieldType::TrackFieldsBoundingResult::Size>
struct IPackedTracks {
    using Container = std::vector<std::array<std::string_view, S>>;
    Container OKTracks;
    Container missingFieldsTracks;
};

struct IScanner {
 public:
    virtual void scanFill(const std::string_view &source, std::size_t &pos, TrackFieldsBoundingResult &result) const = 0;
    virtual constexpr const std::string_view& fieldName() const = 0;
};

template<unsigned int IndexT, StringLiteral FieldName, StringLiteral LBegin, StringLiteral LEnd>
struct FieldTypeStruct : public IScanner {

 // make sure any FieldTypeStruct respects boundaries
 static_assert(IndexT < TrackFieldsBoundingResult::Size);

 public:
    constexpr FieldTypeStruct() {}

    void scanFill(const std::string_view &source, std::size_t &pos, TrackFieldsBoundingResult &result) const final {       
        //
        auto &trackField = result.trackFields[index];
        auto &missingField = result.missingFields[index];

        //  
        auto foundBegin = avx2_find(source, _beginFP, pos);
        if(foundBegin == std::string::npos) {
            missingField = true;
            return;
        }

        //
        foundBegin += _beginFP.size();
        pos = foundBegin;

        // 
        auto foundEnd = avx2_find(source, _endFP, pos);
        if(foundEnd == std::string::npos) {
            missingField = true;
            return;
        }

        //
        auto dataFieldLength = foundEnd - foundBegin;
        pos += dataFieldLength + _endFP.size();

        //
        trackField = std::string_view { 
            source.begin() + foundBegin, 
            dataFieldLength
        };
    }
 
    constexpr const std::string_view& fieldName() const final {
        return _fieldName;
    }

    static constexpr const auto _fieldName = std::string_view { FieldName.value, sizeof(FieldName.value) - 1 };

    static constexpr const unsigned int index = IndexT;

 private:
    static constexpr const auto _beginFP = std::string_view { LBegin.value, sizeof(LBegin.value) - 1 };
    static constexpr const auto _endFP = std::string_view { LEnd.value, sizeof(LEnd.value) - 1 };
};

constexpr FieldTypeStruct<0, "Track ID",       "<key>Track ID</key><integer>",       "</integer>">   TrackID;
constexpr FieldTypeStruct<1, "Disc Number",    "<key>Disc Number</key><integer>",    "</integer>">   DiscNumber;
constexpr FieldTypeStruct<2, "Track Number",   "<key>Track Number</key><integer>",   "</integer>">   TrackNumber;
constexpr FieldTypeStruct<3, "Year",           "<key>Year</key><integer>",           "</integer>">   Year;
constexpr FieldTypeStruct<4, "Date Added",     "<key>Date Added</key><date>",        "</date>">      DateAdded;
constexpr FieldTypeStruct<5, "Name",           "<key>Name</key><string>",            "</string>">    Name;
constexpr FieldTypeStruct<6, "Album Artist",   "<key>Album Artist</key><string>",    "</string>">    AlbumArtist;
constexpr FieldTypeStruct<7, "Album",          "<key>Album</key><string>",           "</string>">    Album;
constexpr FieldTypeStruct<8, "Genre",          "<key>Genre</key><string>",           "</string>">    Genre;

constexpr std::array<const IScanner*, TrackFieldsBoundingResult::Size> orderedScans {
    &TrackID,
    &DiscNumber,
    &TrackNumber,
    &Year,
    &DateAdded,
    &Name,
    &AlbumArtist,
    &Album,
    &Genre,
};

}