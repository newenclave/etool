#include "etool/queues/condition/simple.h"

#include "catch.hpp"

using namespace etool;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using namespace std::chrono_literals;

TEST_CASE("The delayed queue", "[delayed queue]")
{

    queues::condition::simple<std::function<void()>> queue;
    int test = 0;

    SECTION("regular")
    {
    }
}
