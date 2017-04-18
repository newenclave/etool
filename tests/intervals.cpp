#include <iostream>

#include "etool/intervals/set.h"
#include "etool/intervals/map.h"

#include "etool/intervals/traits/vector_set.h"
#include "etool/intervals/traits/vector_map.h"

#include "catch.hpp"

using namespace etool;

using ival_type = intervals::set<double,
                  intervals::traits::vector_set<double> >;
using F = ival_type::position::factory;

TEST_CASE( "Intervals with +inf, -inf" ) {

    intervals::set<double> sint;

    SECTION( "with -inf and +inf" ) {
        sint.insert( F::minus_infinite( ) );
        sint.insert( F::plus_infinite( ) );
        REQUIRE( sint.size( ) == 2 );
        CHECK( sint.to_string( ) == "(-inf, -inf) (+inf, +inf)" );
    }

    SECTION( "-inf and +inf could be overlapped with (-inf, +inf)" ) {
        sint.insert( F::minus_infinite( ) );
        sint.insert( F::plus_infinite( ) );
        sint.insert( F::infinite( ) );

        REQUIRE( sint.size( ) == 1 );
        CHECK( sint.to_string( ) == "(-inf, +inf)" );
    }

    SECTION( "(-inf, +inf) can be splitted with empty interval (0, 0]" ) {
        sint.insert( F::infinite( ) );
        sint.cut( F::left_open( 0, 0 ) );

        REQUIRE( sint.size( ) == 2 );
        CHECK( sint.to_string( ) == "(-inf, 0] (0, +inf)" );
    }
}

TEST_CASE( "Intervals can be overlapped" ) {

    intervals::set<double, intervals::traits::vector_set<double> > sint;

    SECTION( "With insert we get 3 disfferent set" ) {
        sint.insert( 0.0,  100.0 );
        sint.insert( 10.0, 10.0001 );
        REQUIRE( sint.size( ) == 3 );
    }

    SECTION( "We cant insert empty set" ) {
        sint.insert( F::closed( 1.0,  100.0 ) );
        sint.insert( F::open(1.0,  1.0) );
        sint.insert( F::left_closed(1.0,  1.0) );
        sint.insert( F::right_closed(1.0,  1.0) );
        REQUIRE( sint.size( ) == 1 );
        REQUIRE( sint.to_string( ) == "[1, 100]" );
    }
}