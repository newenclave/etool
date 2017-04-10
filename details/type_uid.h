#ifndef TYPE_UID_H
#define TYPE_UID_H

#include <cstdint>

namespace etool { namespace details {

    template <typename T>
    struct type_uid;

    template <typename T>
    struct type_uid {
        static std::uintptr_t get( )
        {
            static const std::uintptr_t data = 0xDeadCe11;
            return reinterpret_cast<std::uintptr_t>(&data);
        }
    };

    /// remove const
    template <typename T>
    struct type_uid<const T> {
        static std::uintptr_t get( )
        {
            return type_uid<T>::get( );
        }
    };

    /// remove pointer
    template <typename T>
    struct type_uid<T *> {
        static std::uintptr_t get( )
        {
            return type_uid<T>::get( );
        }
    };

    /// remove reference
    template <typename T>
    struct type_uid<T &> {
        static std::uintptr_t get( )
        {
            return type_uid<T>::get( );
        }
    };

}}

#endif // TYPE_UID_H
