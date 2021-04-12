#include "ITunesLibraryParser.hpp"
#include "Mesurable.hpp"

int main() {

    // make measurements more precise
    Measurable::makeMorePrecise();

    auto m = Measurable { "Parse XML ITunes Library File into JSON" };

        const auto parser = ITunesLibraryParser { 
            "D:/Musique/iTunes/iTunes Music Library.xml",
            "output.json",
            "warnings.json"
        };

        parser.produceOutputs();

    m.printElapsed();

    return 0;
}
