#include "Mesurable.hpp"
#include "TracksData.hpp"

int main() {
    auto m = Mesurable {"XML file memory copy"};
        const auto xmlLib = ITunesXMLLibrary { "D:/Musique/iTunes/iTunes Music Library.xml" };
    m.printElapsedMs();

    m = {"Get infile track boundaries"};
        const auto boundaries = TracksBoundaries { xmlLib };
    m.printElapsedMs();

    m = {"Get infile end-track positions (multi-threaded)"};
        const auto rawTracks = RawTracksCollection { boundaries };
    m.printElapsedMs();

    std::cout << ">> Tracks found : " << rawTracks.size() << '\n';

    m = {"Find tracks data elements positions (multi-threaded)"};
        const auto tracksData = TracksData { rawTracks };
    m.printElapsedMs();

    constexpr std::string_view numP = "1";
    for(const auto &trackData : tracksData) {
        for(auto i = 0; i < trackData.missingFields.size(); ++i) {
            if(trackData.missingFields[i] == false) continue;

        }
    }

    return 0;
}
