#pragma once

#include <assert.h>

#include <sstream>
#include <fstream>
#include <string>

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
class JSONParser :  public IPipeableSource<JSONParser<T>>, 
                    public std::ostringstream {
 public:
    JSONParser(FieldType::IPackedTracks<>::Container &&);

    JSONParser(JSONParser&&) = default;
    JSONParser(const JSONParser&) = delete;
    void operator=(const JSONParser&) = delete;

    ~JSONParser() {}

    void copyToFile(const char * filePath, const char* descr) {
        auto m = Measurable { descr };   
            std::ofstream { filePath, std::ifstream::out | std::ifstream::trunc | std::ifstream::binary } << this->str();
        m.printElapsedMs();
    }

    void escapeUnsafe() {
        // TODO
    }
};

using MissingFieldsJSONParser = JSONParser<ParseFileType::MissingFields>;
using SuccessfulJSONParser = JSONParser<ParseFileType::Successful>;

//
//
//

template<>
SuccessfulJSONParser::JSONParser(FieldType::IPackedTracks<>::Container &&tracks) : IPipeableSource(this) {
    //
    constexpr auto fieldsC = FieldType::orderedScans.size();
    const auto tracksC = tracks.size();

    *this << '[';

    for(auto i = 0; i < tracksC; ++i) {
        //
        *this << '{';

            // field
            for(auto y = 0; y < fieldsC; ++y) {
                //
                *this << '"';
                *this << FieldType::orderedScans[y]->fieldName(); 
                *this << "\":\"";
                *this << tracks[i][y];
                *this << '"';

                // conditionnal join
                if(y < fieldsC - 1) 
                    *this << ',';
            }

        //
        *this << '}';

        // conditionnal join
        if(i < tracksC - 1)
            *this << ',';
    }

    //
    *this << ']';
};

template<>
MissingFieldsJSONParser::JSONParser(FieldType::IPackedTracks<>::Container &&tracks) : IPipeableSource(this) {
    //
    Padding padding;
    
    //
    const auto fieldsC = FieldType::orderedScans.size();
    const auto tracksC = tracks.size();

    *this << '['<< '\n';

    padding.incr();
    
    for(auto i = 0; i < tracksC; ++i) {
        //
        *this << padding << '{' << '\n';

        //
        padding.incr();

            // field
            for(auto y = 0; y < fieldsC; ++y) {
                //
                *this << padding;

                //
                *this << '"';
                *this << FieldType::orderedScans[y]->fieldName(); 
                *this << "\":\"";
                *this << tracks[i][y];
                *this << '"';

                // conditionnal join
                if(y < fieldsC - 1) 
                    *this << ",\n";
            }

        //
        padding.decr();

        //
        *this << '\n' << padding << "}";

        // conditionnal join
        if(i < tracksC - 1)
            *this << ",\n";
    }

    //
    padding.decr();

    //
    *this << '\n' << ']';
};
