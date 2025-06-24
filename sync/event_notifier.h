#pragma once

#include "absl/synchronization/mutex.h"

namespace zeta {

class EventNotifier {
public:
    EventNotifier() : condition_(canWake, this) {}

    void Wait();

    void WaitWithTimeout(absl::Duration duration);

    void Notify();

private:
    static bool canWake(void* ctx);

    absl::Mutex mutex_;
    absl::Condition condition_;
    std::atomic<bool> can_waked_;
};

}