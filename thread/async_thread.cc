
#include "thread/async_thread.h"

#include <pthread.h>
#include <string>

#include "absl/synchronization/blocking_counter.h"

namespace zeta {

static void SetThreadName(const std::string& name) {
    if (name.empty()) return;
    constexpr size_t MAX_NAME_LENGTH = 15;
    char truncatedName[MAX_NAME_LENGTH + 1];
    strncpy(truncatedName, name.c_str(), MAX_NAME_LENGTH);
    truncatedName[MAX_NAME_LENGTH] = '\0';

#ifdef __APPLE__
    pthread_setname_np(truncatedName);
#else
// Linux 实现
#if (__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 12)
    pthread_setname_np(pthread_self(), truncatedName);
#else
    prctl(PR_SET_NAME, truncatedName, 0, 0, 0);
#endif
#endif
}

bool AsyncThreadImpl::Init(const std::string& name) {
    thread_name_ = name;
    return true;
}

static thread_local AsyncThread* sCurrThread = nullptr;

AsyncThread* GetCurrThread() {
    return sCurrThread;
}

bool AsyncThreadImpl::Start() {
    thread_.reset(new std::thread([this]() {
        sCurrThread = this;
        loopInternal();
        sCurrThread = nullptr;
    }));
    return true;
}

bool AsyncThreadImpl::Stop() {
    absl::BlockingCounter counter(1);
    Invoke([&counter, this]() {
        is_stop_ = true;
        counter.DecrementCount();
    });
    counter.Wait();

    thread_->join();
    thread_.reset();
    return true;
}

void AsyncThreadImpl::invokeInternal(std::function<void()> &&func, Closure<void>* task) {
    {
        absl::MutexLock lock(&mutex_);
        tasks_.emplace_back();
        tasks_.back().func_ = std::move(func);
        tasks_.back().closure_ = task;
    }
    notifier_.Notify();
}

void AsyncThreadImpl::Invoke(Closure<void>* task) {
    invokeInternal(nullptr, task);
}

void AsyncThreadImpl::Invoke(std::function<void()>&& func) {
    invokeInternal(std::move(func), nullptr);
}

void AsyncThreadImpl::invokeLaterInternal(
                         uint64_t delay_us,
                         std::function<void()> &&func,
                         Closure<void>* cl,
                         std::function<void()>&& cancel) {
    auto invoke_ts = absl::Now() + absl::Microseconds(delay_us);

    TimerTask* task = new TimerTask;
    task->timer_invoke_ts_ = invoke_ts;
    task->task_.func_ = std::move(func);
    task->task_.closure_ = cl;
    task->cancel_func_ = std::move(cancel);
    Invoke([this, task]() {
        delay_tasks_.emplace(task);
    });
}

void AsyncThreadImpl::InvokeLater(uint64_t delay_us, Closure<void>* task, std::function<void()>&& cancel) {
    invokeLaterInternal(delay_us, nullptr, task, std::move(cancel));
}

void AsyncThreadImpl::InvokeLater(uint64_t delay_us, std::function<void()>&& callback, std::function<void()>&& cancel) {
    invokeLaterInternal(delay_us, std::move(callback), nullptr, std::move(cancel));
}

bool AsyncThreadImpl::loopOnce(absl::Time now) {
    if (!delay_tasks_.empty() && delay_tasks_.top()->timer_invoke_ts_ <= now) {
        delay_tasks_.top()->task_.Run();
        auto p = delay_tasks_.top();
        delay_tasks_.pop();
        delete p;
        return true;
    }

    Task task;
    {
        absl::MutexLock lock(&mutex_);
        if (tasks_.empty()) {
            return false;
        }
        task = std::move(tasks_.front());
        tasks_.pop_front();
    }
    task.Run();
    return true;
}

void AsyncThreadImpl::loopInternal() {
    SetThreadName(thread_name_);
    while (!is_stop_) {
        auto current_ts = absl::Now();
        if (loopOnce(current_ts)) {
            continue;
        }

        absl::Duration d;
        if (getWaitDuration(current_ts, &d)) {
            notifier_.WaitWithTimeout(d);
        } else {
            notifier_.Wait();
        }
    }
}

AsyncThreadImpl::~AsyncThreadImpl() {
    while (!delay_tasks_.empty()) {
        auto t = delay_tasks_.top();
        delay_tasks_.pop();

        if (t->cancel_func_ != nullptr) {
            t->cancel_func_();
        }

        delete t;
    }
}

}  // namespace zeta