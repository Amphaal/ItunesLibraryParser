#pragma once

#include <cxxabi.h>

#include "Mesurable.hpp"

template<typename Self>
struct IPipeableSource {
 public:
    IPipeableSource(Self* p) : _p(p) {}

    ~IPipeableSource() {
        std::cout << "[] << " << _demangle(typeid(Self).name()) << "\n";
    }

    static std::string _demangle(char const* mangled) {
        auto ptr = std::unique_ptr<char, decltype(& std::free)>{
            abi::__cxa_demangle(mangled, nullptr, nullptr, nullptr),
            std::free
        };
        return {ptr.get()};
    }

    template<typename Target>
    Target pipe(const char* measurementDescr) const {
        auto m = Measurable { measurementDescr };
            auto out = Target { *_p };
        m.printElapsed();
        return out;
    }

    template<typename Target>
    Target pipeMove(const char* measurementDescr) const {
        auto m = Measurable { measurementDescr };
            auto out = Target { std::move(*_p) };
        m.printElapsed();
        return out;
    }

    template<void (Self::*MemberFunction) ()>
    Self& execTrace(const char* measurementDescr) {
        auto m = Measurable { measurementDescr };
            (_p->*MemberFunction)();
        m.printElapsed();
        return *_p;
    }

    Self* _p;
};
