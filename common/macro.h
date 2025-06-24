#pragma once

namespace zeta {

template <typename F>
struct ScopeGuard {
    F f_;
    ScopeGuard(F f) : f_(f) {}
    ~ScopeGuard() { f_(); }
};

template <typename F>
ScopeGuard<F> makeGuard(F f) {
    return ScopeGuard<F>(f);
}
} // namespace zeta

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x)    DEFER_2(x, __COUNTER__)
#define ZETA_DEFER(code)   auto DEFER_3(_defer_) = zeta::makeGuard([&](){code;})

#undef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(clazz) \
    clazz(const clazz&) = delete; \
    clazz& operator=(const clazz&) = delete;
