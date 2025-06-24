#pragma once

#include <functional>
#include <list>
#include <queue>
#include <thread>

#include "sync/event_notifier.h"

#include "base/closure.h"
#include "common/macro.h"

namespace zeta {

class AsyncThread {
public:
    AsyncThread() {}

    virtual ~AsyncThread() {}

    virtual bool Init(const std::string& name) = 0;

    virtual bool Start() = 0;

    virtual bool Stop() = 0;

    virtual void Invoke(Closure<void>* task) = 0;

    virtual void Invoke(std::function<void()>&& callback) = 0;

    virtual void InvokeLater(uint64_t delay_us, Closure<void>* task, std::function<void()>&& cancel = nullptr) = 0;;

    virtual void InvokeLater(uint64_t delay_us, std::function<void()>&& callback, std::function<void()>&& cancel = nullptr) = 0;
private:
    DISALLOW_COPY_AND_ASSIGN(AsyncThread);
};

AsyncThread* GetCurrThread();

class AsyncThreadImpl : public AsyncThread {
public:
    AsyncThreadImpl() {}

    ~AsyncThreadImpl();

    bool Init(const std::string& name) override ;

    bool Start() override;

    bool Stop() override;

    void Invoke(Closure<void>* task) override;

    void Invoke(std::function<void()>&& callback) override;

    void InvokeLater(uint64_t delay_us, Closure<void>* task, std::function<void()>&& cancel = nullptr) override;

    void InvokeLater(uint64_t delay_us, std::function<void()>&& callback, std::function<void()>&& cancel = nullptr) override;

private:
    void invokeInternal(std::function<void()> &&func, Closure<void>* task);

    void invokeLaterInternal(uint64_t delay_us,
                             std::function<void()> &&func,
                             Closure<void>* task,
                             std::function<void()>&& cancel);

    bool getWaitDuration(absl::Time now, absl::Duration* d) const {
        if (delay_tasks_.empty()) {
            return false;
        }
        *d = delay_tasks_.top()->timer_invoke_ts_ - now;
        return true;
    }

    bool loopOnce(absl::Time now);
    void loopInternal();


    struct Task {
        std::function<void()> func_;
        Closure<void>* closure_ = nullptr;

        void Run() const {
            if (closure_ != nullptr) {
                closure_->Run();
                return;
            }
            if (func_ != nullptr) {
                func_();
            }
        }
    };

    class TimerTask {
    public:
        void Cancel();

    private:
        absl::Time timer_invoke_ts_;
        Task task_;
        AsyncThread* th_;
        std::function<void()> cancel_func_;
        friend AsyncThreadImpl;
    };

    struct Cmp {
        bool operator () (const TimerTask* lhs, const TimerTask* rhs) const {
            //  lhs < rhs
            return rhs->timer_invoke_ts_ < lhs->timer_invoke_ts_;
        }
    };

    EventNotifier notifier_;
    absl::Mutex mutex_;
    std::list<Task> tasks_;   // Protected by mutex_

    std::unique_ptr<std::thread> thread_;

    //  is_stop_ and delay_tasks_ only access by thread
    std::priority_queue<TimerTask *, std::deque<TimerTask *>, Cmp> delay_tasks_;
    bool is_stop_ = false;

    std::string thread_name_;
};

}  // namespace zeta


