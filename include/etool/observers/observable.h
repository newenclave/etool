#ifndef ETOOL_OBSERVERS_OBSERVABLE_H
#define ETOOL_OBSERVERS_OBSERVABLE_H

#include "etool/observers/simple.h"
#include <algorithm>

namespace etool { namespace observers {

    template <typename T>
    class obserevable {
    public:
        using value_type = T;

        obserevable() = default;
        obserevable(obserevable&&) = default;
        obserevable(const obserevable&) = default;
        obserevable& operator=(obserevable&&) = default;
        obserevable& operator=(const obserevable&) = default;

        template <typename... Args>
        obserevable(Args&&... args)
            : value_(std::forward<Args>(args)...)
        {
        }

        void swap(value_type& other)
        {
            std::swap(other.value_, value_);
        }

    private:
        value_type value_;
    };

}}

#endif