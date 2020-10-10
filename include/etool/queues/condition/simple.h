#ifndef ETOOL_CONDITION_QUEUE_SIMPLE_H
#define ETOOL_CONDITION_QUEUE_SIMPLE_H

#include "etool/queues/condition/base.h"
#include "etool/queues/condition/traits/simple.h"

namespace etool { namespace queues { namespace condition {
    template <typename T>
    using simple = base<traits::simple<T>>;
}}}

#endif // SIMPLE_H
