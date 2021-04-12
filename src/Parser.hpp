#pragma once

#include <assert.h>

#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include "PackedTracks.hpp"

template<std::size_t EstMaxTrackLength>
struct OneWayBuffer {
 public:
    OneWayBuffer(const std::size_t tracksCount) : 
        _size(tracksCount * EstMaxTrackLength) {
            assert(_size);
            _buffer = (char*)malloc(_size);
        }

    ~OneWayBuffer() {
        free(_buffer);
    }

    OneWayBuffer& operator<<(const char& c) {
        _buffer[_index] = c;
        _index++;
        return *this;
    }

    OneWayBuffer& operator<<(const std::string_view& view) {
        for(const auto &c : view) {
            *this << c;
        }
        return *this;
    }

    const std::size_t& tellp() const {
        return _index;
    }

    const std::string_view str() const {
        return std::string_view { _buffer, _eofIndex };
    }

    void seekp(std::size_t pos) {
        _index = pos;
    }

    void eof() {
        _eofIndex = _index;
    }

 private:
    std::size_t _size;
    std::size_t _index = 0;
    std::size_t _eofIndex = 0;
    char* _buffer;
};

struct Padding : public std::string {
 public:
    void incr() {
        *this += '\t';
    }

    void decr() {
        this->erase(this->end() - 1, this->end());
    }
};

enum class ParseFileType {
    MissingFields,
    Successful
};

template<ParseFileType T, std::size_t EstMaxTrackLength>
struct JSONParser {
 public:
    JSONParser(FieldType::OutputContainer &&);
    
    ~JSONParser() {}
    JSONParser(const JSONParser&) = delete;
    void operator=(const JSONParser&) = delete;

    void copyToFile(const char * filePath) const {
        std::ofstream { filePath, std::ifstream::out | std::ifstream::trunc | std::ifstream::binary } << _memFileStream.str();
    }

    void lfQuotesToEscape(const std::string_view &toSearch, const std::size_t &sPos) {
        //
        std::size_t bPos = 0;
        std::size_t foundAt;
        static constexpr const auto toEscape = '\"';
        
        //
        while(true) {
            //
            foundAt = toSearch.find(toEscape, bPos); 
            if(foundAt == std::string_view::npos) return;

            //
            _dblQuotesPosToEscape.emplace_back(sPos + foundAt);
            bPos = foundAt + sizeof(toEscape);
        }
    }

 
 private:
    OneWayBuffer<EstMaxTrackLength> _memFileStream;
    std::vector<std::size_t> _dblQuotesPosToEscape;

    void _escapeUnsafe() {
        //
        for(const auto &characterToEscape : _dblQuotesPosToEscape) {
            _memFileStream.seekp(characterToEscape);
            _memFileStream << '\'';
        }

        // prevent further escaping !
        _dblQuotesPosToEscape.clear();
    }

};

using MissingFieldsJSONParser = JSONParser<ParseFileType::MissingFields, 500>;
using SuccessfulJSONParser    = JSONParser<ParseFileType::Successful,    350>;

//
//
//

template<>
SuccessfulJSONParser::JSONParser(FieldType::OutputContainer &&tracks) : _memFileStream{tracks.size()} {
    //
    constexpr auto fieldsC = FieldType::orderedScans.size();
    const auto tracksC = tracks.size();
    auto &output = _memFileStream;

    output << '[';

    for(auto i = 0; i < tracksC; ++i) {
        //
        output << '{';

            // field
            for(auto y = 0; y < fieldsC; ++y) {

                //
                output << '"';
                output << FieldType::orderedScans[y]->fieldName(); 
                output << "\":\"";

                //
                auto pos = output.tellp();
                lfQuotesToEscape(tracks[i][y], pos);

                //
                output << tracks[i][y];
                output << '"';

                // conditionnal join
                if(y < fieldsC - 1) 
                    output << ',';
            }

        //
        output << '}';

        // conditionnal join
        if(i < tracksC - 1)
            output << ',';
    }

    //
    output << ']';

    //
    output.eof();

    //
    _escapeUnsafe();
};

template<>
MissingFieldsJSONParser::JSONParser(FieldType::OutputContainer &&tracks) : _memFileStream{tracks.size()} {
    //
    Padding padding;
    
    //
    auto &output = _memFileStream;
    const auto fieldsC = FieldType::orderedScans.size();
    const auto tracksC = tracks.size();

    output << '['<< '\n';

    padding.incr();
    
    for(auto i = 0; i < tracksC; ++i) {
        //
        output << padding << '{' << '\n';

        //
        padding.incr();

            // field
            for(auto y = 0; y < fieldsC; ++y) {
                //
                output << padding;

                //
                output << '"';
                output << FieldType::orderedScans[y]->fieldName(); 
                output << "\":\"";

                //
                auto pos = output.tellp();
                lfQuotesToEscape(tracks[i][y], pos);

                //
                output << tracks[i][y];
                output << '"';

                // conditionnal join
                if(y < fieldsC - 1) 
                    output << ",\n";
            }

        //
        padding.decr();

        //
        output << '\n' << padding << "}";

        // conditionnal join
        if(i < tracksC - 1)
            output << ",\n";
    }

    //
    padding.decr();

    //
    output << '\n' << ']';

    //
    output.eof();

    //
    _escapeUnsafe();
};
