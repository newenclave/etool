#ifndef ETOOL_CACHE_TRAITS_UNIQUE_H
#define ETOOL_CACHE_TRAITS_UNIQUE_H

#include <memory>

namespace etool { namespace cache {

namespace traits {

    template <typename T>
    struct unique {

        typedef std::unique_ptr<T> value_type;

        template <typename ...Args>
        static
        value_type create( Args && ...args )
        {
            value_type res( new T(std::forward<Args>(args)...) );
            return res;
        }

        static
        void destroy( value_type )
        {  }

    };
}

}}

#endif // SHARED_H
