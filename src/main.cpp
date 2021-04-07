#include "Parser.hpp"

#include <cstdio>

int main() {
    // make measurements more precise
    Measurable::makeMorePrecise();

    // xml file to read
    constexpr auto destLib = std::string_view { "D:/Musique/iTunes/iTunes Music Library.xml" };

    // file read in memory, to keep alive until parsing is done
    auto inMemoryFile = IPipeableSource(&destLib)
        .pipe<ITunesXMLLibrary>                                         ("XML file memory copy");
    
    // process to get packed tracks, used for file parsing
    auto packedTracks = inMemoryFile       
        .pipe<TracksBoundaries>                                         ("Get infile track boundaries")
        .pipeMove<const RawTracksCollection>                            ("Get infile end-track positions (multi-threaded)")
        .pipeMove<TracksBoundingResult>                                 ("Find tracks data elements positions (multi-threaded)")
            .execTrace<&TracksBoundingResult::fillDefaultingValues>     ("Fill defaulting values")
        .pipeMove<PackedTracks>                                         ("Pack tracks into bundles for parsing");

    // TODO prevent '"' character breaking

    // //
    // IPipeableSource(&packedTracks.missingFieldsTracks)
    //     .pipeMove<MissingFieldsJSONParser>                              ("Generate warnings tracks manifest")
    //     .execTrace<&MissingFieldsJSONParser::escapeUnsafe>              ("Escape warnings tracks manifest quotes")
    //     .copyToFile                                   ("warnings.json", "create [warnings.json] manifest");

    // //
    // IPipeableSource(&packedTracks.OKTracks)
    //     .pipeMove<SuccessfulJSONParser>                                 ("Generate successful tracks manifest")
    //     .execTrace<&SuccessfulJSONParser::escapeUnsafe>                 ("Escape successful tracks manifest quotes")
    //     .copyToFile                                     ("output.json", "create [output.json] manifest");

    return 0;
}
