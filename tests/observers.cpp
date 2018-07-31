#include "etool/observers/simple.h"
#include <iostream>
#include <mutex>

#include "catch.hpp"

using namespace etool;

TEST_CASE("Observers", "[observer]")
{

    observers::simple<void()> observer;
    SECTION("simple call")
    {
        int test = 0;
        observer.subscribe([&]() { test++; });
        observer();
        REQUIRE(test == 1);
    }
    SECTION("call can be made from slots")
    {
        int test = 0;
        observer.subscribe([&]() {
            ++test;
            if (1 == test) {
                observer();
            }
        });
        observer();
        REQUIRE(test == 2);
    }
    SECTION("subscribtions")
    {
        int test = 0;
        auto ss = observer.subscribe([&]() { ++test; });
        observer();
        observer();
        ss.unsubscribe();
        observer();
        REQUIRE(test == 2);
    }
    SECTION("scoped subscribtion")
    {
        int test = 0;
        {
            observers::scoped_subscription ss
                = observer.subscribe([&]() { ++test; });
            observer();
        }
        observer();
        REQUIRE(test == 1);
    }
    SECTION("subscribe in calls")
    {
        int test = 0;
        auto call = [&]() { test++; };
        observer.subscribe([&]() {
            test++;
            if (1 == test) {
                observer.subscribe(call);
            }
        });
        observer();
        observer();
        REQUIRE(test == 3);
    }
}
