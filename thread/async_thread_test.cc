
#include "thread/async_thread.h"

#include <string>

#include "gtest/gtest.h"
#include "sync/wait_group.h"
#include "base/lambda_closure.h"

namespace zeta {

TEST(AsyncThread, basic) {
    zeta::AsyncThreadImpl thread;
    thread.Init("test");
    thread.Start();
    ZETA_DEFER(thread.Stop());

    WaitGroup wg;

    std::vector<int> vs;
    wg.Add(3);
    thread.Invoke([&] () {
        vs.push_back(1);
        wg.Done();
    });


    thread.InvokeLater(1ULL * 1000 * 1000,
    [&] () {
        vs.push_back(2);
        wg.Done();
    });

    wg.Add(1);
    thread.InvokeLater(5ULL * 100 * 1000,
       NewLambdaClosure([&] (){
        vs.push_back(4);
        wg.Done();
    }));


    thread.Invoke([&] () {
        vs.push_back(3);
        wg.Done();
    });
    wg.Wait();
    ASSERT_EQ(vs[0], 1);
    ASSERT_EQ(vs[1], 3);
    ASSERT_EQ(vs[2], 4);
    ASSERT_EQ(vs[3], 2);

    uint64_t lag = 0;
    LogHeader log_header;
    log_header.lsn_ = 25111243164ull;
    log_header.lag_ = log_header.lsn_;
    std::cerr << log_header.lag_ << std::endl;
    std::cerr << (log_header.lsn_  - log_header.lag_) << std::endl;
}

}  // namespace zeta