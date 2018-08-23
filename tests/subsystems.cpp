#include "etool/subsystems/root.h"

#include "catch.hpp"

using namespace etool;

TEST_CASE("Subsystems", "[root]")
{
    class empty_subsys : public subsystems::iface {
        void init() {}
        void start() {}
        void stop() {}
    };

    subsystems::root subsys_root;
    SECTION("empty root")
    {
        auto empty = subsys_root.get<empty_subsys>();
        REQUIRE(nullptr == empty);
		REQUIRE_THROWS(subsys_root.get_ref<empty_subsys>());
    }

}
