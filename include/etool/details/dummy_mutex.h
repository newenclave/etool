#ifndef ETOOL_DETAIL_DUMMY_MUTEX_H
#define ETOOL_DETAIL_DUMMY_MUTEX_H

namespace etool { namespace details {

    struct dummy_mutex {
        bool try_lock()
        {
            return true;
        }
        void lock() {}
        void unlock() {}
    };

}}

#endif // DUMMY_MUTEX_H
