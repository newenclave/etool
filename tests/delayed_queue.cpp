#include "etool/queues/delayed/simple.h"

#include "catch.hpp"

using namespace etool;
using std::chrono::microseconds;
using std::chrono::milliseconds;

namespace {
    std::chrono::steady_clock::time_point now()
    {
        return std::chrono::steady_clock::now();
    }
}

TEST_CASE( "The delayed queue", "[delayed queue]" ) {

    queues::delayed::simple queue;
    int test = 0;

    SECTION( "regular" ) {
        queue.post_task([&](){ ++test; });
        queue.post_task([&](){ ++test; });
        queue.post_task([&](){ ++test; });
        queue.post_stop();
        REQUIRE(queue.run() == 4);
        REQUIRE(test == 3);
    }
    SECTION("delayed task should not run earlear") {
        int test = 0;
        auto start = now();
        queue.post_delayed_task(microseconds(100), [&]() { test++; });
        queue.run(2);
        auto stop = now();
        REQUIRE(test == 1);
        REQUIRE((stop - start) >= microseconds(100));
    }
	SECTION("all regular task shod be called in proper order") {
		std::vector<int> results;
		for (int i = 0; i < 10; i++) {
			queue.post_task([&results, i]() { results.push_back(i); });
		}
		REQUIRE(results == std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
	}
}

