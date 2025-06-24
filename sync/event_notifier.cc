
#include "sync/event_notifier.h"

namespace zeta {

void EventNotifier::Wait() {
    {
        absl::MutexLock locker(&mutex_);
        mutex_.Await(condition_);
    }
    can_waked_ = false;
}

void EventNotifier::WaitWithTimeout(absl::Duration duration) {
    {
        absl::MutexLock locker(&mutex_);
        mutex_.AwaitWithTimeout(condition_, duration);
    }
    can_waked_ = false;
}

void EventNotifier::Notify() {
    absl::MutexLock locker(&mutex_);
    can_waked_ = true;
}

bool EventNotifier::canWake(void* ctx) {
    EventNotifier* c = static_cast<EventNotifier*>(ctx);
    return c->can_waked_;
}

}