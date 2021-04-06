#pragma once

#include <assert.h>

#include <fstream>
#include <string>

#include "PackedTracks.hpp"

struct Padding : public std::string {
    void incr() {
        *this += '\t';
    }

    void decr() {
        this->erase(this->end() - 1, this->end());
    }
};

void parseIntoJSON_CompleteFields(const char* destFilePath, const FieldType::IPackedTracks<>::Container &OKTracks) {
    
    //
    auto output = std::ofstream { destFilePath, std::ifstream::out | std::ifstream::trunc | std::ifstream::binary };
    assert(output.good());

    //
    constexpr auto fieldsC = FieldType::orderedScans.size();
    const auto tracksC = OKTracks.size();

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
                output << OKTracks[i][y];
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
};

void parseIntoJSON_MissingFields(const char* destFilePath, const FieldType::IPackedTracks<>::Container &missingFieldsTracks) {
    //
    Padding padding;
    auto outputWarning = std::ofstream { destFilePath, std::ifstream::out | std::ifstream::trunc | std::ifstream::binary };
    assert(outputWarning.good());
    
    //
    const auto fieldsC = FieldType::orderedScans.size();
    const auto tracksC = missingFieldsTracks.size();

    outputWarning << '['<< '\n';

    padding.incr();
    
    for(auto i = 0; i < tracksC; ++i) {
        //
        outputWarning << padding << '{' << '\n';

        //
        padding.incr();

            // field
            for(auto y = 0; y < fieldsC; ++y) {
                //
                outputWarning << padding;

                //
                outputWarning << '"';
                outputWarning << FieldType::orderedScans[y]->fieldName(); 
                outputWarning << "\":\"";
                outputWarning << missingFieldsTracks[i][y];
                outputWarning << '"';

                // conditionnal join
                if(y < fieldsC - 1) 
                    outputWarning << ",\n";
            }

        //
        padding.decr();

        //
        outputWarning << '\n' << padding << "}";

        // conditionnal join
        if(i < tracksC - 1)
            outputWarning << ",\n";
    }

    //
    padding.decr();

    //
    outputWarning << '\n' << ']';
};