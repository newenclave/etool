#include "etool/sizepack/fixint.h"
#include "etool/sizepack/zigzag.h"

#include "catch.hpp"

using namespace etool;

TEST_CASE("Fixed Integer", "[fixints]")
{
    SECTION("pack unpack")
    {
        using pack_type = sizepack::fixint<std::uint32_t>;
        std::uint32_t test_data = 0xC8F681A3;
        std::string result;
        pack_type::pack(test_data, result);
        REQUIRE(result.size() == pack_type::min_length);

        std::uint32_t res_data
            = pack_type::unpack(result.begin(), result.end());
        REQUIRE(res_data == test_data);
    }

    SECTION("zig-zag") {}
}
