#ifndef ETOOL_DELAYED_QUEUE_TRAIT_STL_CONDITIONAL_H
#define ETOOL_DELAYED_QUEUE_TRAIT_STL_CONDITIONAL_H

#include <mutex>
#include <condition_variable>

namespace etool { namespace queues { namespace delayed {

    namespace traits {
        struct stl_condition {
            using mutex_type = std::mutex;
            using condition_variable_type = std::condition_variable;
        };
    };
}}}

#endif