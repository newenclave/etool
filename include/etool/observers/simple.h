#ifndef ETOOL_OBSERVERS_SIMPLE_H
#define ETOOL_OBSERVERS_SIMPLE_H

#include "etool/observers/base.h"
#include "etool/observers/fbase.h"
#include "etool/observers/traits/simple.h"

namespace etool { namespace observers {

    template <typename T, typename MutexType = std::recursive_mutex>
    using simple = base<traits::simple<T>, MutexType>;

    template <typename T, typename MutexType = std::mutex>
    using fsimple = fbase<traits::simple<T>, MutexType>;

}}

#endif // SIMPLE_H
