#include "etool/intervals/set.h"
#include "etool/intervals/map.h"

#include "etool/intervals/traits/vector_set.h"
#include "etool/intervals/traits/vector_map.h"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

using namespace etool;

using ival_set = intervals::set<double,
                  intervals::traits::vector_set<double> >;
using F = ival_set::position::factory;

int main(  int argc, char* argv[ ] )
{
    int result = Catch::Session().run( argc, argv );
    return ( result < 0xff ? result : 0xff );
}
