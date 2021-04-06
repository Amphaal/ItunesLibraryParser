#include "Parser.hpp"

#include <cstdio>

int main() {
    // make measurements more precise
    Measurable::makeMorePrecise();

    // xml file to read
    constexpr auto destLib = std::string_view { "D:/Musique/iTunes/iTunes Music Library.xml" };

    // file read in memory, to keep alive until parsing is done
    auto inMemoryFile = IPipeableSource(&destLib)
        .pipe<ITunesXMLLibrary>                                                         ("XML file memory copy");
    
    // process to get packed tracks, used for file parsing
    auto packedTracks = inMemoryFile       
        .pipe<TracksBoundaries>                                                         ("Get infile track boundaries")
        .pipeMove<const RawTracksCollection>                                            ("Get infile end-track positions (multi-threaded)")
        .pipeMove<TracksBoundingResult>                                                 ("Find tracks data elements positions (multi-threaded)")
            .execTrace<&TracksBoundingResult::fillDefaultingValuesOnMissingFields>      ("Fill defaulting values")
        .pipeMove<const PackedTracks>                                                   ("Pack tracks into bundles for parsing");

    // TODO prevent '"' character breaking
    // TODO use http://www.fastformat.org/ for fast writing ?

    //
    auto m = Measurable { "Generate warnings tracks manifest" };
        parseIntoJSON_MissingFields("warnings.json", packedTracks.missingFieldsTracks);
    m.printElapsedMs();

    //
    m = Measurable { "Generate tracks manifest" };
        parseIntoJSON_CompleteFields("output.json", packedTracks.OKTracks);
    m.printElapsedMs();

    return 0;
}
