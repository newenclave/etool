#ifndef ETOOL_DETAILS_ALIGNED_BUFFER_H
#define ETOOL_DETAILS_ALIGNED_BUFFER_H

#include <cstdint>
#include <utility>

namespace etool { namespace details {

    template <typename ValueT, std::size_t AlignVal = alignof(ValueT)>
    struct alignas(AlignVal) aligned_buffer {

    private:
        using this_type = aligned_buffer<ValueT>;

    public:
        using value_type = ValueT;
        using pointer = value_type*;
        using referefce = value_type&;

        static_assert(sizeof(value_type) > 0, "value_type has size == 0");

        enum { is_array = 0, size = 1 };

        using value_buffer = std::uint8_t[sizeof(ValueT)];

        pointer get() noexcept
        {
            return reinterpret_cast<pointer>(value);
        }

        const pointer get() const noexcept
        {
            return reinterpret_cast<const pointer>(value);
        }

        struct scoped_new {

            scoped_new(scoped_new&&) = delete;
            scoped_new(const scoped_new&) = delete;
            scoped_new& operator=(scoped_new&&) = delete;
            scoped_new& operator=(const scoped_new&) = delete;

            scoped_new() = default;

            ~scoped_new()
            {
                if (ptr_) {
                    destroy(ptr_);
                }
            }

            template <typename... ArgT>
            scoped_new(value_type* v, ArgT... args)
                : ptr_(v)
            {
                new (v) value_type(std::forward<ArgT>(args)...);
            }

            template <typename... ArgT>
            static void create(value_type* v, ArgT... args)
            {
                new (v) value_type(std::forward<ArgT>(args)...);
            }

            static void destroy(value_type* v)
            {
                v->~value_type();
            }

            value_type* release() noexcept
            {
                auto tmp = ptr_;
                ptr_ = nullptr;
                return tmp;
            }

        private:
            value_type* ptr_ = nullptr;
        };

    private:
        value_buffer value;
    };

    template <typename ValueT, std::size_t N, std::size_t AlignVal>
    struct alignas(AlignVal) aligned_buffer<ValueT[N], AlignVal> {

    private:
        using this_type = aligned_buffer<ValueT[N]>;

    public:
        static_assert(N > 0, "N must be greater then 0.");

        using value_type = ValueT;
        using value_buffer = std::uint8_t[sizeof(value_type) * N];
        using pointer = value_type*;
        using referefce = value_type&;

        enum { is_array = 1, size = N };

        value_type& operator[](std::size_t pos) noexcept
        {
            return get()[pos];
        }

        const value_type& operator[](std::size_t pos) const noexcept
        {
            return get()[pos];
        }

        value_type* get() noexcept
        {
            return reinterpret_cast<value_type*>(value);
        }

        const value_type* get() const noexcept
        {
            return reinterpret_cast<const value_type*>(value);
        }

        struct scoped_new {

            scoped_new(scoped_new&&) = delete;
            scoped_new(const scoped_new&) = delete;
            scoped_new& operator=(scoped_new&&) = delete;
            scoped_new& operator=(const scoped_new&) = delete;

            scoped_new() = default;

            ~scoped_new()
            {
                if (ptr_) {
                    destroy(ptr_);
                }
            }

            template <typename... ArgT>
            scoped_new(value_type* v, ArgT... args)
                : ptr_(v)
            {
                create(ptr_, std::forward<ArgT>(args)...);
            }

            template <typename... ArgT>
            static void create(value_type* v, ArgT... args)
            {
                for (std::size_t i = 0; i < N; ++i) {
                    new (&v[i]) value_type(std::forward<ArgT>(args)...);
                }
            }

            static void destroy(value_type* v)
            {
                for (std::size_t i = 0; i < N; ++i) {
                    v[i].~value_type();
                }
            }

            value_type* release() noexcept
            {
                auto tmp = ptr_;
                ptr_ = nullptr;
                return tmp;
            }

        private:
            value_type* ptr_ = nullptr;
        };

    private:
        value_buffer value;
    };

}}

#endif // ALIGNED_BUFFER_H
