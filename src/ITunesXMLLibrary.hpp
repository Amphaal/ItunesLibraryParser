#pragma once

#include <stdio.h>
#include <assert.h>

#include <filesystem>
#include <string_view>

#include "helpers/Pipeable.hpp"

class ITunesLibraryFileName : public std::filesystem::path {
 public:
    ITunesLibraryFileName(const char * filePath) : std::filesystem::path(filePath), fileSize(std::filesystem::file_size(*this)) {
        assert(std::filesystem::exists(*this));
        assert(!std::filesystem::is_directory(*this));
    }

    ITunesLibraryFileName(ITunesLibraryFileName&&) = default;
    ITunesLibraryFileName(const ITunesLibraryFileName&) = delete;
    void operator=(const ITunesLibraryFileName&) = delete;

    const uintmax_t fileSize;
};

// ITunes XML Library file stored on memory
struct ITunesXMLLibrary : public IPipeableSource<ITunesXMLLibrary> {
 public:
    const uintmax_t fileSize;
    char* ptr;

    ITunesXMLLibrary(const ITunesLibraryFileName&& filePath) : IPipeableSource(this), fileSize(filePath.fileSize) {
        auto f = fopen(filePath.string().c_str(), "rb");
        assert(f);

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
