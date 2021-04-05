#pragma once

#include <assert.h>

#include <string_view>
#include <vector>
#include <array>

namespace FieldType {

template<size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    
    char value[N];
};

template<unsigned short Size>
struct ParsedTrackResult {
    std::array<std::string_view, Size>  trackFields;
    std::array<bool, Size>              missingFields;
};

template<StringLiteral FieldName, StringLiteral LBegin, StringLiteral LEnd>
struct FieldTypeStruct {
 public:
    static void scanFill(const std::string_view &source, std::size_t &pos, std::string_view &dest, bool& notFound) {       
        //  
        std::size_t foundBegin;
        foundBegin = source.find(_beginFP, pos);
        if(foundBegin == source.npos) {
            notFound = true;
            return;
        }
        foundBegin += _beginFPSize;
        pos += foundBegin;

        // 
        std::size_t foundEnd;
        foundEnd = source.find(_endFP, pos);
        if(foundEnd == source.npos) {
            notFound = true;
            return;
        }
        pos += foundEnd + _endFPSize;

        //
        dest = std::string_view { 
            source.begin() + foundBegin, 
            foundEnd - foundBegin 
        };
    }
 
 private:
    static constexpr const char* _fieldName = FieldName.value;
    static constexpr const char* _beginFP = LBegin.value;
    static constexpr const char* _endFP = LEnd.value;

    static constexpr auto _beginFPSize = sizeof(LBegin.value);
    static constexpr auto _endFPSize = sizeof(LEnd.value);
};

constexpr FieldTypeStruct<"Track ID",       "<key>Track ID</key><integer>",       "</integer>">   TrackID;
constexpr FieldTypeStruct<"Track Number",   "<key>Track Number</key><integer>",   "</integer>">   TrackNumber;
constexpr FieldTypeStruct<"Year",           "<key>Year</key><integer>",           "</integer>">   Year;
constexpr FieldTypeStruct<"Date Added",     "<key>Date Added</key><date>",        "</date>">      DateAdded;
constexpr FieldTypeStruct<"Name",           "<key>Name</key><string>",            "</string>">    Name;
constexpr FieldTypeStruct<"Album Artist",   "<key>Album Artist</key><string>",    "</string>">    AlbumArtist;
constexpr FieldTypeStruct<"Album",          "<key>Album</key><string>",           "</string>">    Album;
constexpr FieldTypeStruct<"Genre",          "<key>Genre</key><string>",           "</string>">    Genre;

}

typedef FieldType::ParsedTrackResult<8> ITracksData;