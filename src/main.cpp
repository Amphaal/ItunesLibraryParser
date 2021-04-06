#include "Mesurable.hpp"
#include "TracksData.hpp"

#include <cstdio>

int main() {
    std::ios_base::sync_with_stdio(false);

    auto m = Measurable { "XML file memory copy" };
        const auto xmlLib = ITunesXMLLibrary { "D:/Musique/iTunes/iTunes Music Library.xml" };
    m.printElapsedMs();

    m = { "Get infile track boundaries" };
        auto boundaries = TracksBoundaries { xmlLib };
    m.printElapsedMs();

    m = { "Get infile end-track positions (multi-threaded)" };
        const auto rawTracks = RawTracksCollection { std::move(boundaries) };
    m.printElapsedMs();

    std::cout << ">> Tracks found : " << rawTracks.size() << '\n';

    m = { "Find tracks data elements positions (multi-threaded)" };
        auto tracksData = TracksData { std::move(rawTracks) };
    m.printElapsedMs();

    m = { "Fill defaulting values" };
        tracksData.fillDefaultingValuesOnMissingFields();
    m.printElapsedMs();

    // TODO : parse output JSON File
    // TODO : parse warning File
    // TODO : auto-fill Disc Number

    return 0;
}
