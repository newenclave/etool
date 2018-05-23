#ifndef ETOOL_CACHE_TRAITS_SHARED_H
#define ETOOL_CACHE_TRAITS_SHARED_H

#include <memory>

namespace etool { namespace cache { namespace traits {

    template <typename T> struct shared {

        typedef std::shared_ptr<T> value_type;

        template <typename... Args> static value_type create(Args&&... args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        static void destroy(value_type) {}
    };
}}}

#endif // SHARED_H
