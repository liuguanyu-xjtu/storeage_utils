#pragma once

#include "absl/synchronization/mutex.h"

namespace zeta {

class WaitGroup {
public:
    WaitGroup() : condition_(&isDone, this) {}

    void Add(int delta) {
        absl::MutexLock lock(&mutex_);
        wait_count_ += delta;
    }

    void Done() {
        Add(-1);
    }

    void Wait() {
        absl::MutexLock locker(&mutex_);
        mutex_.Await(condition_);
    }

    int GetCount() const {
        absl::MutexLock locker(&mutex_);
        return wait_count_;
    }

private:
    static bool isDone(void* ctx) {
        WaitGroup* wg = static_cast<WaitGroup *>(ctx);
        return wg->wait_count_ == 0;
    }

    mutable absl::Mutex mutex_;
    absl::Condition condition_;
    uint32_t wait_count_ = 0;
};

}