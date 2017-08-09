
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "etool/intervals/set.h"
#include "etool/intervals/map.h"
#include "etool/intervals/value.h"

#include "etool/observers/simple.h"
#include "etool/observers/define.h"
#include "etool/details/type_uid.h"
#include "etool/details/lexical_cast.h"
#include "etool/slices/memory.h"

#include "etool/charset/utf8.h"

using namespace etool;

using str_slice = slices::memory<const char>;

void perm (int v[], int n, int i)
{
    int j;
    if (i == n) {
        for (j=0; j<n; j++) printf ("%d ", v[j]);
        printf ("\n");
    } else {
        for (j=i; j<n; j++) {
            std::swap (v[i], v[j]);
            perm(v, n, i + 1);
            std::swap (v[i], v[j]);
        }
    }
}

int main(  int /*argc*/, char** /*argv[ ]*/ )
{
    int v[5], i;

    for (i=0; i<5; i++) v[i] = i+1;
    perm (v, 5, 0);
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

