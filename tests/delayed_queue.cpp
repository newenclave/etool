#include "etool/queues/delayed/simple.h"

#include "catch.hpp"

using namespace etool;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using namespace std::chrono_literals;

namespace {

auto now()
{
    return std::chrono::steady_clock::now();
}
}

TEST_CASE("The delayed queue", "[delayed queue]")
{

    queues::delayed::simple queue;
    int test = 0;

    SECTION("regular")
    {
        queue.post_task([&]() { ++test; });
        queue.post_task([&]() { ++test; });
        queue.post_task([&]() { ++test; });
        queue.post_stop();
        queue.post_task([&]() { ++test; });
        REQUIRE(queue.run() == 4);
        REQUIRE(test == 3);
    }

    SECTION("delayed task should not run earlear")
    {
        int test = 0;
        auto start = now();
        queue.post_delayed_task([&]() { test++; }, 100us);
        queue.run(2);
        auto stop = now();
        REQUIRE(test == 1);
        REQUIRE((stop - start) >= 100us);
    }

    SECTION("all regular task should be called in proper order")
    {
        std::vector<int> results;
        for (int i = 0; i < 10; i++) {
            queue.post_task([&results, i]() { results.push_back(i); });
        }
        queue.run(10);
        REQUIRE(results == std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });
    }

    SECTION("add a task inside a task")
    {
        int test = 0;
        auto inc = [&]() {
            test++;
            queue.post_stop();
        };
        auto inc_add = [&]() {
            test++;
            queue.post_task(inc);
        };
        queue.post_task(inc_add);
        /*
            There should be 3 tasks ran.
            1 - inc_add
            2 - inc
            3 - stop
        */
        REQUIRE(3 == queue.run());
        REQUIRE(2 == test);
    }

    SECTION("canceling task")
    {
        int test = 0;
        auto task_id = queue.post_delayed_task([&]() { ++test; }, 1s);
        queue.post_stop();
        REQUIRE(task_id.is_active());
        task_id.cancel();
        REQUIRE_FALSE(task_id.is_active());
        queue.run();
        REQUIRE(0 == test);
    }
}
