#ifndef ETOOL_OBSERVERS_OBSERVABLE_H
#define ETOOL_OBSERVERS_OBSERVABLE_H

#include "etool/observers/simple.h"
#include <algorithm>

namespace etool { namespace observers {

    template <typename T>
    class observable {
    public:
        using value_type = T;

        observable() = default;
        observable(observable&&) = default;
        observable(const observable&) = default;
        observable& operator=(observable&&) = default;
        observable& operator=(const observable&) = default;

        template <typename... Args>
        observable(Args&&... args)
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
