#ifndef ETOOL_DETAILS_ALIGNED_BUFFER_H
#define ETOOL_DETAILS_ALIGNED_BUFFER_H

#include <cstdint>
#include <utility>

namespace etool { namespace details {

    template <typename ValueT>
    struct alignas(alignof(ValueT)) aligned_buffer {

        static_assert( sizeof(ValueT) > 0, "ValueT has size == 0" );

        enum { is_array = 0, size = 1 };

        using value_buffer = std::uint8_t[sizeof(ValueT)];

        ValueT *get( )
        {
            return reinterpret_cast<ValueT *>(value);
        }

        const ValueT *get( ) const
        {
            return reinterpret_cast<const ValueT *>(value);
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

    template <typename ValueT, unsigned N>
    struct alignas(alignof(ValueT)) aligned_buffer<ValueT[N]> {

        static_assert( N > 0, "N must be greater then 0." );

        enum { is_array = 1, size = N };

        using value_buffer = std::uint8_t[sizeof(ValueT) * N];
        ValueT *get( )
        {
            return reinterpret_cast<ValueT *>(value);
        }

        const ValueT *get( ) const
        {
            return reinterpret_cast<const ValueT *>(value);
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
