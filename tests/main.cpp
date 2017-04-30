
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

int main(  int argc, char* argv[ ] )
{
    std::cout << ( 1 <= 0 ) << "\n";
    return 0;
}

int main0(  int argc, char* argv[ ] )
{
    int result = Catch::Session( ).run( argc, argv );
    return ( result < 0xff ? result : 0xff );
}

