#ifndef ETOOL_CHACHE_NONE_H
#define ETOOL_CHACHE_NONE_H

#include "etool/details/dummy_mutex.h"
#include <memory>

namespace etool { namespace cache {

    template <typename T> class none {
    public:
        using elment_type = T;
        using value_type = std::shared_ptr<T>;
        using mutex_type = etool::details::dummy_mutex;

        none(int) {}
        none() = default;
        none(const none&) = default;

        template <typename... Args> value_type get(Args&&... args)
        {
            return std::make_shared<elment_type>(std::forward<Args>(args)...);
        }

        void push(value_type) {}

        size_t size() const
        {
            return 0;
        }
    };

}}

#endif // NONE_H
