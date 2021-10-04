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

#include <iostream>
#include <chrono>

struct Measurable {
 public:
    using Clock = std::chrono::steady_clock;
    Measurable(const char * mesureDescription) :
        _start_tp(Clock::now()),
        _mesureDescription(mesureDescription) {}

    void printElapsed() {
        auto elapsed = _elapsedNs();
        _cumulated += elapsed;
        _printMs(_mesureDescription, elapsed);
    }

    static void printCumulated() {
        std::cout << "||> ";
        _printMs("TOTAL", _cumulated);
    }

    static void makeMorePrecise() {
        std::ios_base::sync_with_stdio(false);
    }

 private:
    static void _printMs(
        const char * description,
        const std::chrono::nanoseconds& timeElapsed) {
        //
        std::cout
        << "["
        << std::chrono::duration <double, std::milli> (timeElapsed).count()
        << "ms"
        << "]\t"
        << description
        << '\n';
    }

    Clock::time_point _start_tp;
    const char * _mesureDescription;
    static inline std::chrono::nanoseconds _cumulated;

    const std::chrono::nanoseconds _elapsedNs() const {
        return Clock::now() - _start_tp;
    }
};
