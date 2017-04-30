
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "etool/intervals/set.h"
#include "etool/intervals/map.h"

using namespace etool;

namespace {

    using u64 = std::uint64_t;

    using ival_type = intervals::interval<u64>;
    using ival_set  = intervals::set<u64>;
    using ival_map  = intervals::map<u64, std::string>;

}

int main0(  int argc, char* argv[ ] )
{
    ival_set is;
    is.insert(ival_type::infinite( ));
    is.insert(ival_type::left_closed( 0, 0 ));

    auto r = is.find(ival_type::left_closed( 0, 0 ));

    return 0;
}

int main(  int argc, char* argv[ ] )
{
    int result = Catch::Session( ).run( argc, argv );
    return ( result < 0xff ? result : 0xff );
}

