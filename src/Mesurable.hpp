#pragma once

#include <iostream>
#include <chrono>

class Measurable {
 public:
    using Clock = std::chrono::steady_clock;
    Measurable(const char * mesureDescription) : 
        _start_tp(Clock::now()), 
        _mesureDescription(mesureDescription) {}

    void printElapsedMs() {
        std::cout << "[" << std::chrono::duration <double, std::milli> (_elapsedNs()).count() << "ms" << "]\t" << _mesureDescription << '\n';
    }

    static void makeMorePrecise() {
        std::ios_base::sync_with_stdio(false);
    }

 private:
    Clock::time_point _start_tp;
    const char * _mesureDescription;

    const std::chrono::nanoseconds _elapsedNs() const {
        return Clock::now() - _start_tp;
    }
};
