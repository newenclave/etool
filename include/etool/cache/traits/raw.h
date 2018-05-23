#ifndef ETOOL_CACHE_TRAITS_RAW_H
#define ETOOL_CACHE_TRAITS_RAW_H

namespace etool { namespace cache { namespace traits {

    template <typename T>
    struct raw {

        typedef T* value_type;

        template <typename... Args>
        static value_type create(Args&&... args)
        {
            return new T(std::forward<Args>(args)...);
        }

        static void destroy(value_type val)
        {
            delete val;
        }
    };
}}}

#endif // RAW_H
