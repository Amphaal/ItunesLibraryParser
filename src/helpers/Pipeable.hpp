#pragma once

#ifdef _DEBUG
#include <cxxabi.h>
#endif

#include "Mesurable.hpp"

template<typename Source>
struct IPipeable {
 public:
    IPipeable(Source* source) : _source(source) {}

    #ifdef _DEBUG
    ~IPipeable() {
        std::cout << "[] << " << _demangle(typeid(Source).name()) << "\n";
    }
    #endif

    template<typename Target>
    Target pipe(const char* measurementDescr) const {
        //
        auto m = Measurable { measurementDescr };
            auto out = Target { *_source };
        m.printElapsed();

        //
        return out;
    }

    template<typename Target>
    Target pipeMove(const char* measurementDescr) const {
        //
        auto m = Measurable { measurementDescr };
            auto out = Target { std::move(*_source) };
        m.printElapsed();

        //
        return out;
    }

    template<void (Source::*MemberFunction) ()>
    Source& execTrace(const char* measurementDescr) {
        //
        auto m = Measurable { measurementDescr };
            (_source->*MemberFunction)();
        m.printElapsed();

        //
        return *_source;
    }

    Source* _source;

 private:
    #ifdef _DEBUG
    static std::string _demangle(char const* mangled) {
        //
        auto ptr = std::unique_ptr<char, decltype(& std::free)>{
            abi::__cxa_demangle(mangled, nullptr, nullptr, nullptr),
            std::free
        };

        //
        return {ptr.get()};
    }
    #endif
};
