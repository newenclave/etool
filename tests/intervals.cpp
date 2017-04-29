#include <cstdint>
#include <random>

#include "etool/intervals/set.h"
#include "etool/intervals/map.h"

#include "catch.hpp"

using namespace etool;

using ival_type = intervals::interval<std::uint64_t>;
using ival_set  = intervals::set<std::uint64_t>;
using ival_map  = intervals::map<std::uint64_t, std::string>;

template <typename T, std::size_t N>
constexpr std::size_t countof(T const (&)[N]) noexcept
{
    return N;
}

TEST_CASE( "SET", "ww") {

    ival_set is;

    SECTION( "+- infinite" ) {
        is.insert(ival_type::minus_infinite( ));
        is.insert(ival_type::plus_infinite( ));
        REQUIRE( is.size( ) == 2 );
    }

    SECTION( "+- infinite 2" ) {
        is.insert(ival_type::minus_infinite( ));
        is.insert(ival_type::plus_infinite( ));
        is.insert(ival_type::infinite( ));
        REQUIRE( is.size( ) == 1 );
    }

}

SCENARIO( "MAP" ) {
    GIVEN( "An interval map" ) {
        ival_map im;
        WHEN( "add interval (-inf, +inf)" ) {
            im.insert( std::make_pair( ival_type::infinite( ), "!") );

            THEN( "map contains the only element" ) {
                REQUIRE( im.size( ) == 1 );
                REQUIRE( im.begin( )->first.to_string( ) == "(-inf, +inf)" );
            }

            THEN( "every value is mapped to '!'" ) {

                std::random_device rd;
                std::uniform_int_distribution<std::uint64_t>
                                   ud( 0, 0xFFFFFFFFFFFFFFFF );

                std::uint64_t values[ ] = {
                    ud( rd ), ud( rd ), ud( rd ), ud( rd ),
                };

                ival_map::iterator f[ ] = {
                    im.find( values[0] ),
                    im.find( values[1] ),
                    im.find( values[2] ),
                    im.find( values[3] ),
                };

                for( auto i = 0; i<countof(f); ++i ) {
                    INFO("Check: " << values[i] );
                    REQUIRE( f[i] != im.end( ) );
                    REQUIRE( f[i]->second == "!" );
                }
            }
            WHEN( "insert some value to the map" ) {
                auto some_value = ival_type::left_closed( 100, 1000 );
                im.insert( std::make_pair( some_value, "@") );
                THEN( "it splis the map into 3 parts" ) {
                    REQUIRE( im.size( ) == 3 );
                }
            }
        }
    }
}

