#include "etool/sizepack/varint.h"

#include "catch.hpp"

using namespace etool;

TEST_CASE("Variadic Integer", "[variint]")
{
    SECTION("pack unpack")
    {
        using pack_type = sizepack::varint<std::uint32_t>;
        std::uint32_t test_data = 0xC8F681A3;
        std::string result;
        pack_type::pack(test_data, result);

        std::uint32_t res_data
            = pack_type::unpack(result.begin(), result.end());
        REQUIRE(res_data == test_data);
    }
}
