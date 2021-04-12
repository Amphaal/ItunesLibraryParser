#pragma once

#include "Parser.hpp"

class ITunesLibraryParser {
 public:
    ITunesLibraryParser(const char * xmlFilePath, const char * outputJSONFilePath, const char * warningJSONFilePath) noexcept : 
    _xmlFilePath(xmlFilePath),
    _outputJSONFilePath(outputJSONFilePath),
    _warningJSONFilePath(warningJSONFilePath) {}

    void produceOutputs() const {
        //
        const auto inMemoryFile = ITunesXMLLibrary { _xmlFilePath };

        //
        auto [OKTracks, missingFieldsTracks] = PackedTracks { 
            RawTracksCollection { 
                inMemoryFile 
            } 
        };

        //
        MissingFieldsJSONParser { std::move(missingFieldsTracks) }
            .copyToFile(_warningJSONFilePath);
        
        //
        SuccessfulJSONParser { std::move(OKTracks) }
            .copyToFile(_outputJSONFilePath);
    }

 private:
    const char * _xmlFilePath;
    const char * _outputJSONFilePath;
    const char * _warningJSONFilePath;
};
