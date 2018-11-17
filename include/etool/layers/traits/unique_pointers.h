#ifndef ETOOL_LAYERS_TRAITS_UNIQUE_POINTERS_H
#define ETOOL_LAYERS_TRAITS_UNIQUE_POINTERS_H

#include <memory>

namespace etool { namespace layers { namespace traits {
    struct unique_pointer {
        template <typename T>
        using pointer_type = std::unique_ptr<T>;

        template <typename T>
        static bool is_empty(const pointer_type<T>& ptr)
        {
            return ptr == nullptr;
        }

        template <typename T>
        static pointer_type<T>& get_write(pointer_type<T>& ptr)
        {
            return ptr;
        }

        template <typename T>
        static void destroy(pointer_type<T>& ptr)
        {
            ptr.reset();
        }

        template <typename T>
        static void swap(pointer_type<T>& lh, pointer_type<T>& rh)
        {
            std::swap(lh, rh);
        }
    };
}}}

#endif
