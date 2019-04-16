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
            auto ss = observer.subscribe_scoped([&]() { ++test; });
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
    SECTION("multiply observers")
    {
        int test = 0;
        auto call = [&]() { test++; };
        std::vector<observers::scoped_subscription> s;
        s.emplace_back(observer.subscribe_scoped(call));
        s.emplace_back(observer.subscribe_scoped(call));
        s.emplace_back(observer.subscribe_scoped(call));
        s.emplace_back(observer.subscribe_scoped(call));
        s.emplace_back(observer.subscribe_scoped(call));
        observer();
        REQUIRE(test == s.size());

        test = 0;
        s[2].unsubscribe();
        observer();
        REQUIRE(test == s.size() - 1);

        test = 0;
        s[3].unsubscribe();
        observer();
        REQUIRE(test == s.size() - 2);
    }
}
