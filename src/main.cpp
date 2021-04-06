#include "PackedTracks.hpp"

#include <fstream>

#include <cstdio>

int main() {
    // make measurements more precise
    Measurable::makeMorePrecise();

    constexpr auto destLib = std::string_view { "D:/Musique/iTunes/iTunes Music Library.xml" };

    auto packedTracks = 
        IPipeableSource(&destLib)
        .pipe<ITunesXMLLibrary>                                                         ("XML file memory copy")
        .pipe<TracksBoundaries>                                                         ("Get infile track boundaries")
        .pipeMove<const RawTracksCollection>                                            ("Get infile end-track positions (multi-threaded)")
        .pipeMove<TracksBoundingResult>                                                 ("Find tracks data elements positions (multi-threaded)")
            .execTrace<&TracksBoundingResult::fillDefaultingValuesOnMissingFields>      ("Fill defaulting values")
        .pipeMove<const PackedTracks>                                                   ("Pack tracks into bundles for parsing");

    // TODO : pack tracks data for parsing
    // TODO : parse output JSON File
    // TODO : parse warning File

    // {
    //     auto outputWarning = std::ofstream { "warnings.json", std::ifstream::out | std::ifstream::trunc | std::ifstream::binary };
    //     outputWarning << '[';
    //     for(const auto &trackData : tracksData) {
    //         outputWarning << '{';
    //         for(auto i = 0; i < FieldType::orderedScans.size(); ++i) {
    //             auto &fieldType = FieldType::orderedScans[i];
    //             trackData.missingFields[]
    //             outputWarning << '{'

    //         }
    //         outputWarning << '}';
    //     }
    //     outputWarning << ']';
    // }

    return 0;
}
