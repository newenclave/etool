#ifndef ETOOL_OBSERVERS_SIMPLE_H
#define ETOOL_OBSERVERS_SIMPLE_H

#include "etool/observers/traits/simple.h"
#include "etool/observers/base.h"

namespace etool { namespace observers {

    template <typename T, typename MutexType = std::recursive_mutex>
    using simple = base<traits::simple<T>, MutexType>;

}}

#endif // SIMPLE_H
