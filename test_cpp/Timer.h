#pragma once

#include <memory>

struct ScopeTimer {
public:
    ScopeTimer();
    explicit ScopeTimer(const char* title);
    ~ScopeTimer();

    ScopeTimer(const ScopeTimer&) = delete;
    ScopeTimer& operator=(const ScopeTimer&) = delete;

private:
    struct pImpl;
    std::unique_ptr<pImpl> _pImpl;
};
