#include "catch.hpp"
#include "etool/trees/trie/base.h"
#include <cctype>
#include <set>

namespace {
using trie_type = etool::trees::trie::base<char, std::string>;

struct caseless_less {
    bool operator()(char lh, char rh) const noexcept
    {
        return std::toupper(lh) < std::toupper(rh);
    }
};
using trie_i_type = etool::trees::trie::base<char, std::string, caseless_less>;

std::string str_toupper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return s;
}

const std::set<std::string> replace_set
    = { "yellow", "red", "green", "blue", "black", "white" };


template <typename TrieType>
TrieType make_replace_trie()
{
    TrieType tt;
    for (auto& v : replace_set) {
        tt.set(v.begin(), v.end(), str_toupper(v));
    }
    return tt;
}

template <typename TrieType>
std::string replace_values(const std::string& data, const TrieType& replacement)
{
    std::string result;
    auto begin = data.cbegin();
    while (begin != data.cend()) {
        auto next = replacement.get(begin, data.cend(), true);
        if (next) {
            result.append(next->begin(), next->end());
            begin = next.end();
        } else {
            result.push_back(*begin++);
        }
    }
    return result;
}

std::string operator "" _s(const char* data, std::size_t)
{
    return data;
}
}

SCENARIO("The Trie", "[trie]")
{
    GIVEN("Some text to replace")
    {
        std::string test = "The colors are yellow, green, blue, red and black. "
                           "But not white";
        std::string expected = "The colors are YELLOW, GREEN, BLUE, RED and "
                               "BLACK. But not WHITE";
        WHEN("replace is called")
        {
            REQUIRE(expected
                    == replace_values(test, make_replace_trie<trie_type>()));
        }
    }
    GIVEN("Some values to remove")
    {
        trie_type test;
        auto value = "123"_s;
        test.set(value, "123");
        test.set("123456", "123456");
        test.set("456"_s, "456");
        WHEN("Remove is called")
        {
            test.remove(value.begin(), value.end());
            auto res = test.get(value.begin(), value.end(), true);
            REQUIRE(!res);
        }

        WHEN("Remove invalid value")
        {
            auto inval = "12"_s;
            test.remove(inval.begin(), inval.end());
            auto res = test.get(value.begin(), value.end(), true);
            REQUIRE(res);
        }
    }
    GIVEN("Caseless trie")
    {
        auto caseless_trie = make_replace_trie<trie_i_type>();
        std::string test = "The colors are yeLloW, GreEN, bLue, ReD and BlAck. "
                           "But not WhiTe";
        std::string expected
            = "The colors are YELLOW, GREEN, BLUE, RED and BLACK. "
              "But not WHITE";

        WHEN("replace is called")
        {
            REQUIRE(expected == replace_values(test, caseless_trie));
        }
    }
}
