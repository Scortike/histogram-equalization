#include "Timer.h"

#include <chrono>
#include <iostream>

struct ScopeTimer::pImpl {
    pImpl()
        : _start(std::chrono::high_resolution_clock::now())
        , _title(nullptr) {
    }

    explicit pImpl(const char* title)
        : _start(std::chrono::high_resolution_clock::now())
        , _title(title) {
    }

    ~pImpl() {
        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start);
        std::cout << (_title ? _title : "") << ": " << duration.count() << " ms\n";
    }

    std::chrono::high_resolution_clock::time_point _start;
    const char* _title;
};

ScopeTimer::ScopeTimer()
    : _pImpl(std::make_unique<pImpl>()) {
}

ScopeTimer::ScopeTimer(const char* title)
    : _pImpl(std::make_unique<pImpl>(title)) {
}

ScopeTimer::~ScopeTimer() = default;
