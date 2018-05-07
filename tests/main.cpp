
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <chrono>
#include <vector>

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
#include "etool/trees/trie/base.h"

int main(int argc, char* argv[])
{
    int result = Catch::Session().run(argc, argv);
    return (result < 0xff ? result : 0xff);
}

