#pragma once

#include <functional>
#include <list>
#include <queue>
#include <thread>

#include "base/closure.h"
#include "common/macro.h"

namespace zeta {

class LambdaClosure : public Closure<void> {
public:
    explicit LambdaClosure(std::function<void()>&& f) { f_ = std::move(f); }

    void Run() override {
        if (f_ != nullptr) {
            f_();
        }
        delete this;
    }

    bool IsSelfDelete() const override { return true; }

private:
    std::function<void()> f_;
    DISALLOW_COPY_AND_ASSIGN(LambdaClosure);
};

inline Closure<void>* NewLambdaClosure(std::function<void()>&& f) {
    return new LambdaClosure(std::move(f));
}


class LambdaBindCallback : public Closure<void> {
public:
    explicit LambdaBindCallback(std::function<void()>&& f) { f_ = std::move(f); }

    LambdaBindCallback() {}

    void Bind(std::function<void()>&& f) { f_ = std::move(f); }

    void Run() override {
        if (f_ != nullptr) {
            f_();
        }
    }

    bool IsSelfDelete() const override { return false; }

private:
    std::function<void()> f_;
    DISALLOW_COPY_AND_ASSIGN(LambdaBindCallback);
};

}