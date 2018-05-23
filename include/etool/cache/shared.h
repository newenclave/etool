#ifndef ETOOL_CHACHE_SHARED_H
#define ETOOL_CHACHE_SHARED_H

#include "etool/cache/simple.h"
#include <memory>
#include <mutex>

namespace etool { namespace cache {

    template <typename T, typename MutexType = std::mutex>
    class shared : public std::enable_shared_from_this<shared<T, MutexType>> {
        struct key {
        };
        typedef cache::simple<T, MutexType> cache_impl;

    public:
        typedef typename cache_impl::value_type value_type;
        typedef typename cache_impl::mutex_type mutex_type;
        typedef std::shared_ptr<shared> shared_type;

        shared(size_t maximum, key)
            : impl_(maximum)
        {
        }

        static shared_type create(size_t maximum = 0)
        {
            return std::make_shared<shared>(maximum, key());
        }

        void clear()
        {
            impl_.clear();
        }

        template <typename... Args>
        value_type get(Args&&... args)
        {
            return impl_.get(std::forward<Args>(args)...);
        }

        void push(value_type val)
        {
            impl_.push(val);
        }

        size_t size() const
        {
            return impl_.size();
        }

    private:
        cache_impl impl_;
    };

}}

#endif // SRPC_COMMON_CHACHE_SHARED_H
