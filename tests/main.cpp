
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

#include "etool/queues/delayed/simple.h"

using namespace etool;

using namespace std::chrono;

std::size_t test_count = 5000000;
std::size_t signal_count = 1;
auto test = 0ul;

int main(int argc, char* argv[])
{
    int result = Catch::Session().run(argc, argv);
    return (result < 0xff ? result : 0xff);
}

namespace {

    queues::delayed::base<queues::delayed::traits::stl_condition> dq;
    void spam()
    {
       std::cout << test++ << "\n";
       dq.post_delayed_task(microseconds(10), spam);
    }
}

int main___()
{
    spam();
    dq.run();
    return 0;
}
