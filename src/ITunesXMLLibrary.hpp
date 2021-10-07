// ItunesLibraryParser
// Allows JSON parsing of XML Itunes Library file
// Copyright (C) 2021 Guillaume Vara <guillaume.vara@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Graphical resources without explicit references to a
// different license and copyright still refer to this GPL.

#pragma once

#include <stdio.h>

#include <cassert>
#include <string>
#include <filesystem>
#include <string_view>

struct ITunesLibraryFileName : public std::filesystem::path {
 public:
    ITunesLibraryFileName(const char * filePath) :
        std::filesystem::path(filePath) {
        //
        if (!std::filesystem::exists(*this) || std::filesystem::is_directory(*this)) {
            std::string m ("Cannot open expected XML Library File" );
            m += " [";
            m += std::filesystem::absolute(*this).string();
            m += "]";
            throw std::logic_error(m);
        }

        //
        fileSize = std::filesystem::file_size(*this);
    }

    ITunesLibraryFileName(const ITunesLibraryFileName&) = delete;
    void operator=(const ITunesLibraryFileName&) = delete;

    uintmax_t fileSize;
};

// ITunes XML Library file stored on memory
struct ITunesXMLLibrary {
 public:
    ITunesXMLLibrary(const ITunesLibraryFileName&& filePath) :
        _fileSize(filePath.fileSize) {
        //
        FILE * f;
        auto err = fopen_s(&f, filePath.string().c_str(), "rb");
        assert(err == 0);

            _ptr = reinterpret_cast<char *>(malloc(_fileSize));

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
