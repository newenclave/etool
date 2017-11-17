#include <iostream>
#include <mutex>
#include "etool/observers/simple.h"

#include "catch.hpp"

using namespace etool;

TEST_CASE("Observers", "[observer]") {

    observers::simple<void()> observer;

    SECTION("simple_call") {
        int test = 0;
        auto call1 = [&]() { if (test++ == 0) { observer.unsubscribe_all(); } };
        auto call2 = [&]() {
            if (++test == 2) {
                observer();
            };
        };
        auto call3 = [&]() { ++test; };
        observer.connect(call1);
        observer.connect(call2);
        observer.connect(call3);
        observer();
        REQUIRE(test == 6);
    }
}
