
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

#include "boost/signals2.hpp"

using namespace etool;

std::size_t test_count = 5000000;
std::size_t signal_count = 1;
auto test = 0ul;
// using mutex_type = std::recursive_mutex;
using mutex_type = boost::signals2::dummy_mutex;


namespace bs {
    const std::string name = "boost";
    using signal = boost::signals2::signal_type<void(void),
        boost::signals2::keywords::mutex_type<mutex_type> >::type;
    signal S;
}

namespace es {
    const std::string name = "etool";
    using signal = observers::simple<void(), mutex_type>;
    signal S;
}

namespace test_ns = bs;

namespace tests {
    template <typename SignalType>
    void simple_call(SignalType &S)
    {
        for (auto i = 0ul; i < signal_count; i++)
        {
            S.connect([&]() {++test; });
        }
        for (auto i = 0ul; i < test_count; ++i)
        {
            S();
        }
    }
}

std::chrono::milliseconds operator "" ms ( unsigned long long d)
{
    return std::chrono::milliseconds(d);
}

std::chrono::hours operator "" h ( unsigned long long d)
{
    return std::chrono::hours(d);
}

int main_(int argc, char* argv[])
{
    std::cout << "Start test for " << test_ns::name << std::endl;
    auto start = std::chrono::high_resolution_clock::now().time_since_epoch();
    tests::simple_call(test_ns::S);
    auto stop = std::chrono::high_resolution_clock::now().time_since_epoch();
    std::cout << "TOTAL: " << test << " in time " << (stop - start).count() << std::endl;
    return 1;
}

int main__(int argc, char* argv[])
{
    int result = Catch::Session().run(argc, argv);
    return (result < 0xff ? result : 0xff);
}

namespace {

    queues::delayed::base<> dq;
    int test = 0;
    void spam()
    {
       std::cout << test++ << "\n";
       dq.post_delayed_task(1000ms, spam);
    }
}

int main ()
{
    auto start = std::chrono::steady_clock::now();
    auto stop = std::chrono::steady_clock::now() + 1000ms;
    std::cout << sizeof(start) << "\n";
    std::cout << (start < stop) << "\n";

    spam();
    dq.run();
}
