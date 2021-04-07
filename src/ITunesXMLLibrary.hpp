#pragma once

#include <stdio.h>
#include <assert.h>

#include <string_view>

#include "Pipeable.hpp"

// ITunes XML Library file stored on memory
struct ITunesXMLLibrary : public IPipeableSource<ITunesXMLLibrary> {
 public:
    long fileSize;
    char* ptr;

    ITunesXMLLibrary(const std::string_view& filePath) : IPipeableSource(this) {
        auto f = fopen(filePath.begin(), "rb");
        assert(f);
            
            fseek(f, 0, SEEK_END); // seek to end of file
            fileSize = ftell(f); // get current file pointer
            fseek(f, 0, SEEK_SET); // seek back to beginning of file

            ptr = (char *)malloc(fileSize);

            fread(ptr, 1, fileSize, f);
        
        fclose(f);
    }

    const std::string_view asStringView() const {
        return std::string_view { ptr, static_cast<std::size_t>(fileSize) };
    }

    ~ITunesXMLLibrary() {
        free(ptr);
    }
};
