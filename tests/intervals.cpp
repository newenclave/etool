#include <iostream>

#include "etool/intervals/set.h"
#include "etool/intervals/map.h"

#include "etool/intervals/traits/vector_set.h"
#include "etool/intervals/traits/vector_map.h"

#include "catch.hpp"

using namespace etool;

using ival_set = intervals::set<double>;
using ival_map = intervals::map<double, std::string>;

using F        = ival_set::position::factory;
using cmp      = ival_set::position::cmp;


TEST_CASE( "Intervals" ) {
    SECTION( "Properties" ) {
        REQUIRE( F::infinite( ).is_max_inf( ) );
        REQUIRE(  cmp::less(F::degenerate(0), F::degenerate(1)) );
        REQUIRE( !cmp::less(F::degenerate(1), F::degenerate(0)) );
    }
}

TEST_CASE( "Intervals with +inf, -inf" ) {

    ival_set sint;

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

        INFO( "The set is " << sint );

        REQUIRE( sint.size( ) == 1 );
    }

    SECTION( "(-inf, +inf) can be splitted with empty interval (0, 0]" ) {
        sint.insert( F::infinite( ) );
        sint.cut( F::left_open( 0, 0 ) );

        REQUIRE( sint.size( ) == 2 );
        CHECK( sint.to_string( ) == "(-inf, 0] (0, +inf)" );

        sint.cut( F::right_open( 0, 0 ) );

        REQUIRE( sint.size( ) == 3 );
        CHECK( sint.to_string( ) == "(-inf, 0) [0, 0] (0, +inf)" );

    }

    SECTION( "(-inf, -inf) (+inf, +inf) can not be cutted off" ) {
        sint.insert( F::infinite( ) );

        REQUIRE( sint.size( ) == 1 );
        CHECK( sint.to_string( ) == "(-inf, +inf)" );

        sint.cut( F::minus_infinite( ) );
        sint.cut( F::plus_infinite( ) );

        REQUIRE( sint.size( ) == 1 );
        CHECK( sint.to_string( ) == "(-inf, +inf)" );
    }

    SECTION( "(-inf, 0) (0, +inf) can be cutted off" ) {
        sint.insert( F::infinite( ) );
        sint.cut( F::right_open( 0 ) );
        sint.cut( F::left_open( 0 ) );

        REQUIRE( sint.size( ) == 1 );
        CHECK( sint.to_string( ) == "[0, 0]" );
    }

}

TEST_CASE( "Intervals can be overlapped" ) {

    intervals::set<double, intervals::traits::vector_set<double> > sint;

    SECTION( "With insert we get 3 disfferent set" ) {
        sint.insert( 0.0,  100.0 );
        sint.insert( 10.0, 10.0001 );
        REQUIRE( sint.size( ) == 3 );
    }

    SECTION( "We can't insert empty set" ) {

        sint.insert( F::closed( 1.0,  100.0 ) );
        INFO( "SET: " << sint );

        INFO( "Inserting " << F::open(1.0,  1.0) );
        sint.insert( F::open(1.0,  1.0) );         /// empty

        INFO( "Inserting " << F::left_closed(1.0,  1.0) );
        sint.insert( F::left_closed(1.0,  1.0) );  /// empty

        INFO( "Inserting " << F::right_closed(1.0,  1.0) );
        sint.insert( F::right_closed(1.0,  1.0) ); /// empty

        REQUIRE( sint.size( ) == 1 );
        REQUIRE( sint.to_string( ) == "[1, 100]" );
    }
}

TEST_CASE( "Interval map can be used as hash-map " ) {

    ival_map fmap;

    SECTION( "interval::map::operator []" ) {

        fmap[0] = "Hello!";
        fmap[1] = "world!";

        CHECK( fmap.to_string( ) == "[0, 0] -> 'Hello!'; [1, 1] -> 'world!'" );

        fmap.insert( F::minus_infinite(  ), "--" );
        fmap.insert( F::plus_infinite(  ),  "++" );

        REQUIRE( fmap.size( ) == 4 );
        CHECK( fmap.to_string( ) == "(-inf, -inf) -> '--'; "
                                    "[0, 0] -> 'Hello!'; "
                                    "[1, 1] -> 'world!'; "
                                    "(+inf, +inf) -> '++'" );
    }

}
