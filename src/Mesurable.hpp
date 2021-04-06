#pragma once

#include <iostream>
#include <chrono>

class Mesurable {
 public:
    Mesurable(const char * mesureDescription) : _start_tp(std::chrono::system_clock::now()), _mesureDescription(mesureDescription) {}
    void printElapsedMs() const {
        std::cout << "[" << _mesureDescription << "] : " << _elapsedNs().count() * 1e-6 << "ms" << '\n';
    }

    void printElapsedNs() const {
        std::cout << "[" << _mesureDescription << "] : " << _elapsedNs().count() << "ns" << '\n';
    }

 private:
    std::chrono::_V2::system_clock::time_point _start_tp;
    const char * _mesureDescription;

    const std::chrono::nanoseconds _elapsedNs() const {
        return std::chrono::system_clock::now() - _start_tp;
    }
};
