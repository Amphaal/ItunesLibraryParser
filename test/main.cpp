#include "Parser.hpp"

int main() {
    // make measurements more precise
    Measurable::makeMorePrecise();

    // xml file to read
    auto m = Measurable { "Check XML file exists" };
        const ITunesLibraryFileName destLib { "D:/Musique/iTunes/iTunes Music Library.xml" };
    m.printElapsed();

    // file read in memory, to keep alive until parsing is done
    auto inMemoryFile = IPipeableSource(&destLib)
        .pipeMove<ITunesXMLLibrary>                                         ("XML file memory copy");
    
    // process to get packed tracks, used for file parsing
    auto packedTracks = inMemoryFile       
        .pipe<TracksBoundaries>                                         ("Get infile track boundaries (AVX2 + std::rfind)")
        .pipeMove<const RawTracksCollection>                            ("Get infile end-track positions (AVX2)")
        .pipeMove<TracksBoundingResult>                                 ("Find tracks data elements positions (AVX2)")
            .execTrace<&TracksBoundingResult::fillDefaultingValues>     ("Fill defaulting values")
        .pipeMove<PackedTracks>                                         ("Pack tracks into bundles for parsing");

    //
    IPipeableSource(&packedTracks.missingFieldsTracks)
        .pipeMove<MissingFieldsJSONParser>                              ("Generate warnings tracks manifest")
        .execTrace<&MissingFieldsJSONParser::escapeUnsafe>              ("Escape warnings tracks manifest quotes")
        .copyToFile("warnings.json",                                     "create [warnings.json] manifest");

    //
    IPipeableSource(&packedTracks.OKTracks)
        .pipeMove<SuccessfulJSONParser>                              ("Generate successful tracks manifest")
        .execTrace<&SuccessfulJSONParser::escapeUnsafe>              ("Escape successful tracks manifest quotes")
        .copyToFile("output.json",                                    "create [output.json] manifest");

    //
    Measurable::printCumulated();

    return 0;
}
