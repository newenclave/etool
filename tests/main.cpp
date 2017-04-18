#include "etool/intervals/set.h"
#include "etool/intervals/map.h"

#include "etool/intervals/traits/vector_set.h"
#include "etool/intervals/traits/vector_map.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace etool;

using ival_type = intervals::set<double,
                  intervals::traits::vector_set<double> >;
using F = ival_type::position::factory;

TEST_CASE( "MAIN" ) { }
