#include "etool/subsystems/root.h"

#include "catch.hpp"

using namespace etool;

namespace {
	class empty_subsys : public subsystems::iface {
		void init() {}
		void start() {}
		void stop() {}
	};

	template <int Id>
	struct test : public subsystems::root {
		test(int *i, int *st, int *stp)
			:init_(i)
			, start_(st)
			, stop_(stp)
		{}
		void init() { (*init_)++; }
		void start() { (*start_)++; }
		void stop() { (*stop_)++; }
		int *init_;
		int *start_;
		int *stop_;
	};
}

TEST_CASE("Subsystems", "[root]")
{
    subsystems::root subsys_root;
    SECTION("empty root")
    {
        auto empty = subsys_root.get<empty_subsys>();
        REQUIRE(nullptr == empty);
        REQUIRE_THROWS(subsys_root.get_ref<empty_subsys>());
    }

	SECTION("root contains root")
	{
		subsys_root.add<subsystems::root>();
		subsys_root.get_ref<subsystems::root>().add<empty_subsys>();
		auto subsys = subsys_root
			.get_ref<subsystems::root>()
			.get<empty_subsys>();
		REQUIRE(nullptr != subsys);
	}

	SECTION("init start stop")
	{
		int init_c = 0;
		int start_c = 0;
		int stop_c = 0;

		subsys_root.add<test<0> >(&init_c, &start_c, &stop_c);
		subsys_root.add<test<1> >(&init_c, &start_c, &stop_c);
		subsys_root.add<test<2> >(&init_c, &start_c, &stop_c);
		REQUIRE(3 == subsys_root.size());
		REQUIRE_NOTHROW(subsys_root.get_ref<test<0> >());
		REQUIRE_NOTHROW(subsys_root.get_ref<test<1> >());
		REQUIRE_NOTHROW(subsys_root.get_ref<test<2> >());
		subsys_root.init();
		subsys_root.start();
		subsys_root.stop();
		REQUIRE(3 == init_c);
		REQUIRE(3 == start_c);
		REQUIRE(3 == stop_c);
	}
}
