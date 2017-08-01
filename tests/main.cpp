
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "etool/intervals/set.h"
#include "etool/intervals/map.h"

#include "etool/observers/simple.h"
#include "etool/observers/define.h"
#include "etool/details/type_uid.h"

#include "etool/charset/utf8.h"

using namespace etool;

int main(  int argc, char* argv[ ] )
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

