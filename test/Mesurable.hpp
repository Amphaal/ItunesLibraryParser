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
    static void _printMs(const char * description, const std::chrono::nanoseconds& timeElapsed) {
        std::cout << "[" << std::chrono::duration <double, std::milli> (timeElapsed).count() << "ms" << "]\t" << description << '\n';
    }

    Clock::time_point _start_tp;
    const char * _mesureDescription;
    static inline std::chrono::nanoseconds _cumulated;

    const std::chrono::nanoseconds _elapsedNs() const {
        return Clock::now() - _start_tp;
    }
};
