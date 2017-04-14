#ifndef ETOOL_DETAILS_MEM_SLICE_H
#define ETOOL_DETAILS_MEM_SLICE_H

#include <cstdint>

namespace etool { namespace details {

    template <typename ValueType, typename SizeType = std::size_t>
    class mem_slice {
    public:

        using value_type     = ValueType;
        using size_type      = SizeType;
        using pointer_type   = value_type *;
        using reference_type = value_type &;

        mem_slice( value_type *value, size_type len )
            :val_(value)
            ,len_(len)
        { }

        mem_slice( value_type *begin, value_type *end )
            :val_(begin)
            ,len_(static_cast<size_type>(end - begin))
        { }

        mem_slice             (                   )  = default;
        mem_slice             ( const mem_slice & )  = default;
        mem_slice             ( mem_slice &&      )  = default;
        mem_slice &operator = ( mem_slice &&      )  = default;
        mem_slice &operator = ( const mem_slice & )  = default;

        template <typename U, typename S = size_type>
        mem_slice<U, S> rebind( )
        {
            S new_size = (sizeof(value_type) * len_) / sizeof(U);
            mem_slice<U, S> tmp( reinterpret_cast<U *>(val_), new_size );
            return tmp;
        }

        template <typename U, typename S = size_type>
        static
        mem_slice bind( U *data, S len )
        {
            mem_slice<U, S> tmp( data, len );
            return tmp.template rebind<value_type, size_type>( );
        }

        template <typename U, typename S = size_type>
        static
        mem_slice bind( mem_slice<U, S> other )
        {
            return other.template rebind<value_type, size_type>( );
        }

        value_type *begin( ) noexcept
        {
            return val_;
        }

        value_type *end( ) noexcept
        {
            return val_ + size( );
        }

        size_type size( ) const noexcept
        {
            return len_;
        }

        void resize( size_type new_size ) noexcept
        {
            len_ = new_size;
        }

        bool empty( ) const noexcept
        {
            return len_ == 0;
        }

        pointer_type get( ) noexcept
        {
            return val_;
        }

        reference_type operator *( ) noexcept
        {
            return *val_;
        }

        mem_slice &operator += ( size_type len ) noexcept
        {
            val_ += len;
            len_ -= len;
            return *this;
        }

        mem_slice &operator -= ( size_type len ) noexcept
        {
            val_ -= len;
            len_ += len;
            return *this;
        }

        mem_slice &operator ++ ( ) noexcept
        {
            val_ ++;
            len_ --;
            return *this;
        }

        mem_slice operator ++ (int) noexcept
        {
            mem_slice tmp( val_, len_ );
            ++(*this);
            return tmp;
        }

        mem_slice &operator -- ( ) noexcept
        {
            val_ --;
            len_ ++;
            return *this;
        }

        mem_slice operator -- (int) noexcept
        {
            mem_slice tmp( val_, len_ );
            --(*this);
            return tmp;
        }

    private:

        value_type  *val_ = nullptr;
        size_type    len_ = 0;
    };

}}

#endif // SLICE_H
