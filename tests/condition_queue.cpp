#include <thread>
#include "etool/queues/condition/simple.h"

#include "catch.hpp"

using namespace etool;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using namespace std::chrono_literals;

TEST_CASE("The condition queue", "[condition queue]")
{

    queues::condition::simple<std::function<void()>> queue;
    int test = 0;

    auto runthread = [&queue]() {
        std::function<void()> value;
        while(queues::condition::QUEUE_WAIT_OK == queue.wait(value)) {
            value();
       }
    };

    SECTION("add values")
    {
        std::thread run(runthread);
        queue.push([&test](){test++;});
        queue.push([&test](){test++;});
        queue.push([&test](){test++;});
        queue.push([&queue](){queue.cancel();});

        run.join();
        REQUIRE(3 == test);
    }
}
