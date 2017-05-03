
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

bool operator < (my_int lh, my_int rh )
{
    std::cout << "less\n";
    return lh.i < rh.i;
}

bool operator == (my_int lh, my_int rh)
{
    std::cout << "eq\n";
    return lh.i == rh.i;
}

bool operator <= (my_int lh, my_int rh)
{
    std::cout << "ls eq\n";
    return lh.i <= rh.i;
}

struct test_cmp {
    bool operator ( ) ( int l, int r ) const
    {
        return l < r;
    }
};

int main0( )
{
    using ival_type = intervals::interval<double>;
    intervals::map<double, std::string> s;

    s.insert(std::make_pair(ival_type::minus_infinite( ), "-inf"));
    s.insert(std::make_pair(ival_type::plus_infinite( ), "+inf"));


    for( auto &a : s ) {
        std::cout << a.first << a.second << "\n";
    }

    return 0;
}

int main(  int argc, char* argv[ ] )
{

    using ival_type = intervals::interval<double>;
    intervals::set<double> s;

    s.insert( ival_type::left_closed( -110, -99 ) );
    s.insert( ival_type::left_closed( 99, 100 ) );

    for( double i = 0; i<10; i += 3 ) {
        s.insert( ival_type::left_closed( i, i + 1 ) );
    }

    auto f = s.find_intersection( ival_type::closed( -100, 2 ) );

    for( auto r = f.first; r != f.second; ++r ) {
        std::cout << *r << "\n";
    }
    std::cout << "==================\n";

    for( auto &a : s ) {
        std::cout << a << "\n";
    }

    return 0;
}

int main1( int argc, char* argv[ ] )
{
    int result = Catch::Session( ).run( argc, argv );
    return ( result < 0xff ? result : 0xff );
}

