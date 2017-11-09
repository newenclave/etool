#include "etool/queues/delayed/base.h"

#include "catch.hpp"

using namespace etool;

TEST_CASE( "The delayed queueu", "[delayed queue]" ) {

    queues::delayed::base queue;
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

