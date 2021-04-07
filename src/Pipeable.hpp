#pragma once

#include "Mesurable.hpp"

template<typename Self>
struct IPipeableSource {
 public:
    IPipeableSource(Self* p) : _p(p) {}

    template<typename Target>
    Target pipe(const char* measurementDescr) const {
        auto m = Measurable { measurementDescr };
            auto out = Target { *_p };
        m.printElapsedMs();
        return out;
    }

    template<typename Target>
    Target pipeMove(const char* measurementDescr) const {
        auto m = Measurable { measurementDescr };
            auto out = Target { std::move(*_p) };
        m.printElapsedMs();
        return out;
    }

    template<void (Self::*MemberFunction) ()>
    Self& execTrace(const char* measurementDescr) {
        auto m = Measurable { measurementDescr };
            (_p->*MemberFunction)();
        m.printElapsedMs();
        return *_p;
    }

    Self* _p;
};
