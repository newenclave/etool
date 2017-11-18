#ifndef ETOOL_DELAYED_QUEUE_SIMPLE_H
#define ETOOL_DELAYED_QUEUE_SIMPLE_H

#include "etool/queues/delayed/base.h"
#include "etool/queues/delayed/traits/stl_condition.h"

namespace etool { namespace queues { namespace delayed {
    using simple = base<traits::stl_condition>;

}}}

#endif // SIMPLE_H
