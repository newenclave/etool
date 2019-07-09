#include <cstdint>
#include <random>

#include "etool/intervals/map.h"
#include "etool/intervals/set.h"

#include "catch.hpp"

using namespace etool;

namespace {

using u64 = std::uint64_t;

using ival_type = intervals::interval<u64>;
using ival_set = intervals::set<u64>;
using ival_map = intervals::map<u64, std::string>;

template <typename T, std::size_t N>
constexpr std::size_t countof(T const (&)[N]) noexcept
{
    return N;
}

std::random_device rd;
std::uniform_int_distribution<std::uint64_t> ud(0, 0xFFFFFFFFFFFFFFFF);

std::uniform_int_distribution<std::uint64_t> ud0(0, 10000000);

std::uniform_int_distribution<std::uint64_t> ud1(10000001, 1000000000);
}

TEST_CASE("The intervals", "[intervals]")
{

    SECTION("infinite")
    {

        auto minus_inf = ival_type::minus_infinite();
        auto plus_inf = ival_type::plus_infinite();
        auto inf = ival_type::infinite();

        REQUIRE(minus_inf.valid());
        REQUIRE(plus_inf.valid());
        REQUIRE(inf.valid());

        SECTION("to_string")
        {
            REQUIRE(minus_inf.to_string() == "(-inf, -inf)");
            REQUIRE(plus_inf.to_string() == "(+inf, +inf)");
            REQUIRE(inf.to_string() == "(-inf, +inf)");
        }

        SECTION("compare lexicographical")
        {
            REQUIRE(minus_inf < plus_inf);
            REQUIRE(!(plus_inf < inf));
            REQUIRE(minus_inf < inf);
        }

        SECTION("compare solid")
        {
            using cmp = ival_type::cmp_not_overlap;

            REQUIRE(cmp::less(minus_inf, plus_inf));
            REQUIRE(!cmp::less(minus_inf, minus_inf));
            REQUIRE(!cmp::less(plus_inf, minus_inf));
            REQUIRE(!cmp::less(minus_inf, inf));
            REQUIRE(!cmp::less(inf, inf));
        }
    }

    SECTION("invalid")
    {
        REQUIRE(ival_type::left_closed(10, 0).invalid());
        REQUIRE(ival_type::left_open(10, 0).invalid());
        REQUIRE(ival_type::closed(10, 0).invalid());
        REQUIRE(ival_type::open(10, 0).invalid());
        REQUIRE(ival_type::open(123, 12).to_string() == "(inval)");
    }

    SECTION("empty")
    {
        REQUIRE(ival_type::left_closed(0, 0).empty());
        REQUIRE(ival_type::left_open(100, 100).empty());
        REQUIRE(ival_type::open(777, 777).empty());
        REQUIRE(!ival_type::closed(9888, 9888).empty());
    }
}

TEST_CASE("SET", "[insert]")
{

    ival_set is;

    SECTION("+- infinite")
    {
        is.insert(ival_type::minus_infinite());
        is.insert(ival_type::plus_infinite());
        REQUIRE(is.size() == 2);
    }

    SECTION("+- infinite 2")
    {
        is.insert(ival_type::minus_infinite());
        is.insert(ival_type::plus_infinite());
        is.insert(ival_type::infinite());
        REQUIRE(is.size() == 1);
    }

    SECTION("merge values")
    {
        u64 maximum = (ud(rd) % 100) + 1;
        for (u64 i = 0; i < maximum; i++) {
            /// "[i , i + 1]"
            is.merge(ival_type::closed(i, i + 1));
        }
        REQUIRE(is.size() == 1);
        REQUIRE(is.begin()->to_string()
                == ival_type::closed(0, maximum).to_string());
    }

    SECTION("absorb values")
    {
        u64 maximum = (ud(rd) % 100) + 1;
        for (u64 i = 0; i < maximum; i++) {
            /// "[i , i + 1)"
            is.absorb(ival_type::left_closed(i, i + 1));
        }
        REQUIRE(is.size() == 1);
        REQUIRE(is.begin()->to_string()
                == ival_type::left_closed(0, maximum).to_string());
    }

    SECTION("empty interval can be inserted")
    {
        is.insert(ival_type::infinite());
        is.insert(ival_type::left_closed(0, 0));
        REQUIRE(is.size() == 3);
        REQUIRE(is.find(ival_type::left_open(0, 0)) != is.end());
        REQUIRE(is.find(ival_type::left_closed(0, 0)) != is.end());
    }

    SECTION("empty interval can be cut")
    {
        is.insert(ival_type::infinite());
        is.cut(ival_type::left_closed(0, 0));
        REQUIRE(is.size() == 2);
        REQUIRE(is.find(ival_type::left_open(0, 0)) == is.end());
        REQUIRE(is.find(ival_type::left_closed(0, 0)) == is.end());
    }
}

SCENARIO("MAP", "[map]")
{

    GIVEN("An interval map")
    {

        ival_map im;

        WHEN("add interval (-inf, +inf)")
        {

            im.insert(std::make_pair(ival_type::infinite(), "!"));

            THEN("map contains the only element")
            {
                REQUIRE(im.size() == 1);
                REQUIRE(im.begin()->first.to_string() == "(-inf, +inf)");
            }

            THEN("every value is mapped to '!'")
            {

                std::uint64_t values[] = {
                    ud(rd),
                    ud(rd),
                    ud(rd),
                    ud(rd),
                };

                ival_map::iterator f[] = {
                    im.find(values[0]),
                    im.find(values[1]),
                    im.find(values[2]),
                    im.find(values[3]),
                };

                for (std::size_t i = 0; i < countof(f); ++i) {
                    INFO("Check: " << values[i]);
                    REQUIRE(f[i] != im.end());
                    REQUIRE(f[i]->second == "!");
                }
            }
            WHEN("insert some value to the map")
            {

                auto l = ud0(rd);
                auto r = ud1(rd);

                INFO("insert " << ival_type::left_closed(l, r));

                auto some_value = ival_type::left_closed(l, r);

                im.insert(std::make_pair(some_value, "@"));
                THEN("it splis the map into 3 parts")
                {
                    REQUIRE(im.size() == 3);
                }
                THEN("2 parts have mapped value '!' and one has '@'")
                {
                    REQUIRE(im.find(l - 1)->second == "!");
                    REQUIRE(im.find(l)->second == "@");
                    REQUIRE(im.find(r - 1)->second == "@");
                    REQUIRE(im.find(r)->second == "!");
                }
            }
            WHEN("cut some value from the map")
            {

                auto l = ud0(rd);
                auto r = ud1(rd);

                INFO("cut " << ival_type::left_closed(l, r));

                auto some_value = ival_type::left_closed(l, r);
                im.cut(some_value);

                THEN("it splis the map into 2 parts")
                {
                    REQUIRE(im.size() == 2);
                }

                THEN("parts have mapped value '!' "
                     "but values from the removed "
                     "interval do not exist")
                {
                    REQUIRE(im.find(l - 1)->second == "!");
                    REQUIRE(im.find(r)->second == "!");

                    REQUIRE(im.find(l) == im.end());
                    REQUIRE(im.find(r - 1) == im.end());
                }
            }

            WHEN("insert and cut some value to the map")
            {
                auto some_value1 = ival_type::left_closed(0, 1000);
                auto some_value2 = ival_type::left_open(100, 500);
                im.insert(std::make_pair(some_value1, "inserted"));
                im.cut(some_value2);

                THEN("it splis the map into 2 parts")
                {
                    REQUIRE(im.size() == 4);
                }
            }

        } /// WHEN

        WHEN("minimum and maximum infinite")
        {
            im[ival_type::minus_infinite()] = "-inf";
            im[ival_type::plus_infinite()] = "+inf";

            THEN("there are no values in map but it is not empty")
            {
                REQUIRE(im.size() == 2);
                REQUIRE(im.find(ud(rd)) == im.end()); /// !!!
                REQUIRE(im.begin()->first.to_string() == "(-inf, -inf)");
                REQUIRE(im.begin()->second == "-inf");

                REQUIRE(std::next(im.begin())->first.to_string()
                        == "(+inf, +inf)");
                REQUIRE(std::next(im.begin())->second == "+inf");
            }
        } // WHEN

    } // GIVEN
}
