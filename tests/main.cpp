
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "etool/intervals/set.h"
#include "etool/intervals/map.h"
#include "etool/intervals/value.h"

#include "etool/observers/simple.h"
#include "etool/observers/define.h"
#include "etool/details/type_uid.h"

#include "etool/charset/utf8.h"

using namespace etool;


int main(  int /*argc*/, char** /*argv[ ]*/ )
{
    auto val1 = intervals::value<int>::make_open( 10 );
    auto val2 = intervals::value<int>::make_closed( 10 );

    auto max_inf = intervals::value<int>::make_max_inf( );
    auto min_inf = intervals::value<int>::make_min_inf( );

    std::cout << std::boolalpha << (val1 <  val2) << "\n";
    std::cout << std::boolalpha << (val2 <  val2) << "\n";
    std::cout << std::boolalpha << (val1 == val2) << "\n";

    std::cout << std::boolalpha << (val1 ==  10) << "\n";
    std::cout << std::boolalpha << (val2 ==  10) << "\n";

    std::cout << std::boolalpha << (val1 < 10) << "\n";
    std::cout << std::boolalpha << (val2 < 10) << "\n";

    std::cout << std::boolalpha << ( 10 < val1 ) << "\n";
    std::cout << std::boolalpha << ( 10 < val2 ) << "\n";

    std::cout << val1 << val2 << "\n";
    std::cout << max_inf << min_inf << "\n";

    return 0;
}

int main0(  int /*argc*/, char** /*argv[ ]*/ )
{
    std::string test = "Hello! йцукен 漢語";
    auto res = charset::utf8::utf8_to_ucs4( test );

    for( auto &r: res ) {
        std::cout << std::hex << r << std::dec << " ";
    }
    std::cout << "\n";

    test = charset::utf8::ucs4_to_utf8( res );
    std::cout << test << "\n";

    return 0;
}

int main1( int argc, char* argv[ ] )
{
    int result = Catch::Session( ).run( argc, argv );
    return ( result < 0xff ? result : 0xff );
}

