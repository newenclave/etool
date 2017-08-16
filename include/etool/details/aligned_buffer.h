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
        using pointer    = value_type *;
        using referefce  = value_type &;

        static_assert( sizeof(ValueT) > 0, "ValueT has size == 0" );

        enum { is_array = 0, size = 1 };

        using value_buffer = std::uint8_t[sizeof(ValueT)];

        pointer get( )
        {
            return reinterpret_cast<pointer>(value);
        }

        const pointer get( ) const
        {
            return reinterpret_cast<const pointer>(value);
        }

        struct scoped_new {

            scoped_new( scoped_new && ) = delete;
            scoped_new( const scoped_new & ) = delete;
            scoped_new& operator = ( scoped_new && ) = delete;
            scoped_new& operator = ( const scoped_new & ) = delete;

            scoped_new( ) = default;

            ~scoped_new(  )
            {
                if( ptr_ ) {
                    destroy( ptr_ );
                }
            }

            template <typename ...ArgT>
            scoped_new( ValueT *v, ArgT...args )
                :ptr_(v)
            {
                new (v) ValueT(std::forward<ArgT>(args)...);
            }

            template <typename ...ArgT>
            static
            void create( ValueT *v, ArgT...args )
            {
                new (v) ValueT(std::forward<ArgT>(args)...);
            }

            static
            void destroy( ValueT *v )
            {
                v->~ValueT( );
            }

            void release( ) noexcept
            {
                ptr_ = nullptr;
            }

        private:

            ValueT * ptr_ = nullptr;
        };

    private:

        value_buffer value;
    };

    template <typename ValueT, unsigned N, std::size_t AlignVal>
    struct alignas(AlignVal) aligned_buffer<ValueT[N], AlignVal> {

    private:
        using this_type = aligned_buffer<ValueT[N]>;
    public:

        static_assert( N > 0, "N must be greater then 0." );

        using value_type    = ValueT;
        using value_buffer  = std::uint8_t[sizeof(value_type) * N];
        using pointer       = value_type *;
        using referefce     = value_type &;

        enum { is_array = 1, size = N };

        value_type & operator [ ](std::size_t pos)
        {
            return get( )[pos];
        }

        const value_type & operator [ ](std::size_t pos) const
        {
            return get( )[pos];
        }

        pointer get( )
        {
            return reinterpret_cast<pointer>(value);
        }

        const pointer get( ) const
        {
            return reinterpret_cast<const pointer>(value);
        }

        struct scoped_new {

            scoped_new( scoped_new && ) = delete;
            scoped_new( const scoped_new & ) = delete;
            scoped_new& operator = ( scoped_new && ) = delete;
            scoped_new& operator = ( const scoped_new & ) = delete;

            scoped_new( ) = default;

            ~scoped_new(  )
            {
                if( ptr_ ) {
                    destroy(ptr_);
                }
            }

            template <typename ...ArgT>
            scoped_new( ValueT *v, ArgT...args )
                :ptr_(v)
            {
                for( unsigned i = 0; i<N; ++i  ) {
                    new (&v[i]) ValueT(std::forward<ArgT>(args)...);
                }
            }

            template <typename ...ArgT>
            static
            void create( ValueT *v, ArgT...args )
            {
                for( auto i = 0; i<N; ++i  ) {
                    new (&v[i]) ValueT(std::forward<ArgT>(args)...);
                }
            }

            static
            void destroy( ValueT *v )
            {
                for( unsigned i = 0; i<N; ++i  ) {
                    v[i].~ValueT( );
                }
            }

            void release( ) noexcept
            {
                ptr_ = nullptr;
            }

        private:
            ValueT * ptr_ = nullptr;
        };

    private:

        value_buffer value;
    };

}}

#endif // ALIGNED_BUFFER_H
