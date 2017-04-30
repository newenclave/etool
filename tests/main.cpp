
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

struct my_int {
    my_int( int ii ) :i(ii) { }
    int i;
};

bool operator < (my_int lh, my_int rh)
{
    std::cout << "less\n";
    return lh.i < rh.i;
}

//bool operator == (my_int lh, my_int rh)
//{
//    std::cout << "eq\n";
//    return lh.i == rh.i;
//}

//bool operator <= (my_int lh, my_int rh)
//{
//    std::cout << "ls eq\n";
//    return lh.i <= rh.i;
//}


int main0(  int argc, char* argv[ ] )
{
    using check_type = my_int;
    using ival = intervals::interval<check_type>;

    std::cout << ival::cmp::greater_equal( my_int(10), my_int(10) )
              << "\n";
    std::cout << ival::cmp::greater_equal( my_int(100), my_int(10) )
              << "\n";
    std::cout << ival::cmp::greater_equal( my_int(1), my_int(10) )
              << "\n";

    return 0;
}

int main( int argc, char* argv[ ] )
{
    int result = Catch::Session( ).run( argc, argv );
    return ( result < 0xff ? result : 0xff );
}

