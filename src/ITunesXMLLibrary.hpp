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
    ITunesXMLLibrary(const ITunesLibraryFileName&& filePath) : IPipeableSource(this), _fileSize(filePath.fileSize) {
        auto f = fopen(filePath.string().c_str(), "rb");
        assert(f);

            _ptr = (char *)malloc(_fileSize);

            fread(_ptr, 1, _fileSize, f);
        
        fclose(f);
    }

    ~ITunesXMLLibrary() {
        free(_ptr);
    }

    const std::string_view asStringView() const {
        return std::string_view { _ptr, static_cast<std::size_t>(_fileSize) };
    }
 
 private:
     const uintmax_t _fileSize;
    char* _ptr;
};
