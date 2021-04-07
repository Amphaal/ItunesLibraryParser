#pragma once

#include <assert.h>

#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include "PackedTracks.hpp"

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

template<ParseFileType T>
class JSONParser :  public IPipeableSource<JSONParser<T>> {
 public:
    JSONParser(FieldType::IPackedTracks<>::Container &&);
    
    JSONParser(JSONParser&&) = delete;
    JSONParser(const JSONParser&) = delete;
    void operator=(const JSONParser&) = delete;

    ~JSONParser() {}

    void copyToFile(const char * filePath, const char* descr) const {
        auto m = Measurable { descr };   
            std::ofstream { filePath, std::ifstream::out | std::ifstream::trunc | std::ifstream::binary } << _memFileStream.str();
        m.printElapsed();
    }

    void lfQuotesToEscape(const std::string_view &toSearch, const std::streampos &sPos) {
        //
        auto bPos = 0;
        std::size_t foundAt;
        
        //
        while(true) {
            //
            foundAt = toSearch.find('\"', bPos); 
            if(foundAt == std::string_view::npos) return;

            //
            _dblQuotesPosToEscape.emplace_back(sPos + foundAt);
            bPos = foundAt + 1;
        }
    }

    void escapeUnsafe() {
        //
        for(const auto &characterToEscape : _dblQuotesPosToEscape) {
            _memFileStream.seekp(characterToEscape);
            _memFileStream << '\'';
        }

        // prevent further escaping !
        _dblQuotesPosToEscape.clear();
    }
 
 private:
    std::ostringstream _memFileStream;
    std::vector<std::size_t> _dblQuotesPosToEscape;
};

using MissingFieldsJSONParser = JSONParser<ParseFileType::MissingFields>;
using SuccessfulJSONParser = JSONParser<ParseFileType::Successful>;

//
//
//

template<>
SuccessfulJSONParser::JSONParser(SuccessfulJSONParser&& s) : 
    IPipeableSource(std::move(s._p)), 
    _memFileStream(std::move(s._memFileStream)) {}

template<>
MissingFieldsJSONParser::JSONParser(MissingFieldsJSONParser&& s) : 
    IPipeableSource(std::move(s._p)), 
    _memFileStream(std::move(s._memFileStream)) {}

template<>
SuccessfulJSONParser::JSONParser(FieldType::IPackedTracks<>::Container &&tracks) : IPipeableSource(this) {
    //
    constexpr auto fieldsC = FieldType::orderedScans.size();
    const auto tracksC = tracks.size();

    _memFileStream << '[';

    for(auto i = 0; i < tracksC; ++i) {
        //
        _memFileStream << '{';

            // field
            for(auto y = 0; y < fieldsC; ++y) {

                //
                _memFileStream << '"';
                _memFileStream << FieldType::orderedScans[y]->fieldName(); 
                _memFileStream << "\":\"";

                //
                auto pos = _memFileStream.tellp();
                lfQuotesToEscape(tracks[i][y], pos);

                //
                _memFileStream << tracks[i][y];
                _memFileStream << '"';

                // conditionnal join
                if(y < fieldsC - 1) 
                    _memFileStream << ',';
            }

        //
        _memFileStream << '}';

        // conditionnal join
        if(i < tracksC - 1)
            _memFileStream << ',';
    }

    //
    _memFileStream << ']';
};

template<>
MissingFieldsJSONParser::JSONParser(FieldType::IPackedTracks<>::Container &&tracks) : IPipeableSource(this) {
    //
    Padding padding;
    
    //
    const auto fieldsC = FieldType::orderedScans.size();
    const auto tracksC = tracks.size();

    _memFileStream << '['<< '\n';

    padding.incr();
    
    for(auto i = 0; i < tracksC; ++i) {
        //
        _memFileStream << padding << '{' << '\n';

        //
        padding.incr();

            // field
            for(auto y = 0; y < fieldsC; ++y) {
                //
                _memFileStream << padding;

                //
                _memFileStream << '"';
                _memFileStream << FieldType::orderedScans[y]->fieldName(); 
                _memFileStream << "\":\"";

                //
                auto pos = _memFileStream.tellp();
                lfQuotesToEscape(tracks[i][y], pos);

                //
                _memFileStream << tracks[i][y];
                _memFileStream << '"';

                // conditionnal join
                if(y < fieldsC - 1) 
                    _memFileStream << ",\n";
            }

        //
        padding.decr();

        //
        _memFileStream << '\n' << padding << "}";

        // conditionnal join
        if(i < tracksC - 1)
            _memFileStream << ",\n";
    }

    //
    padding.decr();

    //
    _memFileStream << '\n' << ']';
};
