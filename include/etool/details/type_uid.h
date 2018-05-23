#ifndef ETOOL_DETAILS_TYPE_UID_H
#define ETOOL_DETAILS_TYPE_UID_H

#include <cstdint>

namespace etool { namespace details {

    template <typename T> struct type_uid;

    template <typename T> struct type_uid {
        enum { is_const = 0, is_ref = 0, is_ptr = 0 };
        static std::uintptr_t get()
        {
            static const std::uintptr_t data = 0xDeadCe11;
            return reinterpret_cast<std::uintptr_t>(&data);
        }
    };

    /// remove const
    template <typename T> struct type_uid<const T> {
        enum { is_const = 1, is_ref = 0, is_ptr = 0 };
        static std::uintptr_t get()
        {
            return type_uid<T>::get();
        }
    };

    /// remove pointer
    template <typename T> struct type_uid<T*> {
        enum { is_const = 0, is_ref = 0, is_ptr = 1 };
        static std::uintptr_t get()
        {
            return type_uid<T>::get();
        }
    };

    template <typename T> struct type_uid<T const*> {
        enum { is_const = 1, is_ref = 0, is_ptr = 1 };
        static std::uintptr_t get()
        {
            return type_uid<T>::get();
        }
    };

    /// remove reference
    template <typename T> struct type_uid<T&> {
        enum { is_const = 0, is_ref = 1, is_ptr = 0 };
        static std::uintptr_t get()
        {
            return type_uid<T>::get();
        }
    };

    template <typename T> struct type_uid<T const&> {
        enum { is_const = 1, is_ref = 1, is_ptr = 0 };
        static std::uintptr_t get()
        {
            return type_uid<T>::get();
        }
    };

}}

#endif // TYPE_UID_H
