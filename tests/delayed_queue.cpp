#include "etool/queues/delayed/simple.h"

#include "catch.hpp"

using namespace etool;

TEST_CASE( "The delayed queue", "[delayed queue]" ) {

    queues::delayed::simple queue;
    int test = 0;

    SECTION( "regular" ) {
        queue.post_task([&](){ ++test; });
        queue.post_task([&](){ ++test; });
        queue.post_task([&](){ ++test; });
        queue.post_stop();
        queue.run();
        REQUIRE(test == 3);
    }
}

