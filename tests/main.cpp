
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <chrono>

#include "etool/intervals/set.h"
#include "etool/intervals/map.h"
#include "etool/intervals/value.h"

#include "etool/observers/simple.h"
#include "etool/observers/define.h"
#include "etool/details/type_uid.h"
#include "etool/details/lexical_cast.h"
#include "etool/slices/memory.h"
#include "etool/details/aligned_buffer.h"
#include "etool/details/result.h"
#include "etool/console/geometry.h"

#include "etool/charset/utf8.h"
#include "etool/details/endian.h"

#include "etool/queues/delayed/base.h"

#include "all.hpp"

using namespace etool;

std::chrono::milliseconds operator "" ms ( unsigned long long d)
{
    return std::chrono::milliseconds(d);
}

std::chrono::hours operator "" h ( unsigned long long d)
{
    return std::chrono::hours(d);
}

using observer = observers::simple<void (void)>;

int main1(  int /*argc*/, char** /*argv[ ]*/ )
{
    observer ob;
    auto ss = ob.subscribe( [&ob]( ) {
        std::cout << "!\n";
    } );
    ob( );
    return 0;
}

int main0(  int /*argc*/, char** /*argv[ ]*/ )
{
    return 0;
}

int main( int argc, char* argv[ ] )
{
    int result = Catch::Session( ).run( argc, argv );
    return ( result < 0xff ? result : 0xff );
}

namespace {
    etool::queues::delayed::base queue;
    std::atomic<std::size_t> counter {0};
}

void sleep_task( )
{
    std::cout << counter << "\n";
    counter = 0;
    queue.post_delayed_task(1000ms, [](){ sleep_task();});
}

void spam_task( )
{
    ++counter ;
    queue.post_delayed_task(0ms, [](){ spam_task(); });
}

int main__()
{
    sleep_task();
    spam_task();
    queue.post_delayed_task(1h, [](){
        std::cout << "Stop!\n";
        queue.stop( );
    });
    queue.run();
    return 1;
}

namespace {
    timertt::default_timer_heap_thread ttq;

    void spam_tt()
    {
        ++counter;
        ttq.activate(0ms, [](){spam_tt();});
    }
    void second_tt()
    {
        std::cout << counter << "\n";
        counter = 0;
        ttq.activate(1000ms, [](){second_tt();});
    }
}


int main_( )
{
    ttq.start();

    spam_tt();
    second_tt();

    //ttq.shutdown();
    ttq.join();
    return 1;
}

