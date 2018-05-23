#ifndef ETOOL_OBSERVERS_DEFINE_H
#define ETOOL_OBSERVERS_DEFINE_H

#include "etool/details/dummy_mutex.h"
#include "etool/observers/base.h"
#include "etool/observers/simple.h"

#include <mutex>

#define ETOOL_OBSERVER_DEFINE_COMMON(Name, Sig, Visible, Mutex)                \
public:                                                                        \
    typedef etool::observers::simple<Sig, Mutex> Name##_type;                  \
    typedef typename Name##_type::slot_type Name##_slot_type;                  \
    etool::observers::subscription subscribe_##Name(Name##_slot_type slot)     \
    {                                                                          \
        return Name.subscribe(slot);                                           \
    }                                                                          \
    Visible:                                                                   \
    Name##_type Name

#define ETOOL_OBSERVER_DEFINE_SAFE(Name, Sig)                                  \
    ETOOL_OBSERVER_DEFINE_COMMON(Name, Sig, protected, std::recursive_mutex)

#define ETOOL_OBSERVER_DEFINE_UNSAFE(Name, Sig)                                \
    ETOOL_OBSERVER_DEFINE_COMMON(Name, Sig, protected,                         \
                                 etool::details::dummy_mutex)

#define ETOOL_OBSERVER_DEFINE ETOOL_OBSERVER_DEFINE_SAFE

#endif // ETOOL_COMMON_OBSERVERS_DEFINE_H
