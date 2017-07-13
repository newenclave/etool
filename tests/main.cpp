
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "etool/intervals/set.h"
#include "etool/intervals/map.h"

#include "etool/observers/simple.h"
#include "etool/observers/define.h"
#include "etool/details/type_uid.h"

using namespace etool;

struct test {
    std::string data;
};

struct cool {
    ETOOL_OBSERVER_DEFINE_COMMON( mutest, void (test &),
                                  public, details::dummy_mutex );
    ETOOL_OBSERVER_DEFINE_COMMON( contest, void (const test &),
                                  public, details::dummy_mutex );
};

void add( test &t )
{
    t.data += std::string("1");
}

void show( const test &t )
{
    std::cout << t.data << "\n";
}

int main(  int argc, char* argv[ ] )
{

    test tt;
    cool c;

    c.subscribe_mutest( add );
    c.subscribe_mutest( add );
    c.subscribe_mutest( add );

    c.subscribe_contest( show );
    c.subscribe_contest( show );
    c.subscribe_contest( show );

    c.mutest( tt );
    c.contest( tt );

    return 0;
}

int main1( int argc, char* argv[ ] )
{
    int result = Catch::Session( ).run( argc, argv );
    return ( result < 0xff ? result : 0xff );
}

