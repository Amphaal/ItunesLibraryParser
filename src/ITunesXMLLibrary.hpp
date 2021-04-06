#pragma once

#include <stdio.h>
#include <assert.h>

#include <string_view>

// ITunes XML Library file stored on memory
struct ITunesXMLLibrary {
    long fileSize;
    char* ptr;

    ITunesXMLLibrary(const char* filePath) {
        auto f = fopen(filePath, "rb");
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
