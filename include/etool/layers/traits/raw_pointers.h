#ifndef ETOOL_LAYERS_TRAITS_RAW_POINTERS_H
#define ETOOL_LAYERS_TRAITS_RAW_POINTERS_H

namespace etool { namespace layers { namespace traits {
    struct raw_pointers {
        template <typename T>
        using pointer_type = T*;

        template <typename T>
        static bool is_empty(pointer_type<T> ptr)
        {
            return ptr == nullptr;
        }

        template <typename T>
        static pointer_type<T> get_write(pointer_type<T> ptr)
        {
            return ptr;
        }

        template <typename T>
        static void destroy(pointer_type<T>)
        {
        }

        template <typename T>
        static void swap(pointer_type<T>& lh, pointer_type<T>& rh)
        {
            std::swap(lh, rh);
        }
    };
}}}

#endif
