#ifndef ETOOL_OBSERVERS_SIMPLE_H
#define ETOOL_OBSERVERS_SIMPLE_H

#include "etool/observers/base.h"
#include "etool/observers/details/impl.h"
#include "etool/observers/details/fimpl.h"

#include "etool/observers/traits/simple.h"

namespace etool { namespace observers {

    template <typename T, typename MutexType = std::recursive_mutex>
    using simple = base<details::impl<traits::simple<T>, MutexType> >;

    template <typename T, typename MutexType = std::recursive_mutex>
    using fsimple = base<details::fimpl<traits::simple<T>, MutexType> >;

}}

#endif // SIMPLE_H
