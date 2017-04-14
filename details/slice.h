#ifndef ETOOL_DETAILS_SLICE_H
#define ETOOL_DETAILS_SLICE_H

#include <cstdint>

namespace etool { namespace details {

    template <typename ValueType, typename SizeType = std::size_t>
    class slice {
    public:

        using value_type     = ValueType;
        using size_type      = SizeType;
        using pointer_type   = value_type *;
        using reference_type = value_type &;

        slice( value_type *value, size_type len )
            :val_(value)
            ,len_(len)
        { }

        slice             (               )  = default;
        slice             ( const slice & )  = default;
        slice             ( slice &&      )  = default;
        slice &operator = ( slice &&      )  = default;
        slice &operator = ( const slice & )  = default;

        template <typename U, typename S = size_type>
        slice<U, S> rebind( )
        {
            S new_size = (sizeof(value_type) * len_) / sizeof(U);
            slice<U, S> tmp( reinterpret_cast<U *>(val_), new_size );
            return tmp;
        }

        template <typename U, typename S = size_type>
        static
        slice bind( U *data, S len )
        {
            slice<U, S> tmp( data, len );
            return tmp.template rebind<value_type, size_type>( );
        }

        template <typename U, typename S = size_type>
        static
        slice bind( slice<U, S> other )
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

        slice &operator += ( size_type len ) noexcept
        {
            val_ += len;
            len_ -= len;
            return *this;
        }

        slice &operator -= ( size_type len ) noexcept
        {
            val_ -= len;
            len_ += len;
            return *this;
        }

        slice &operator ++ ( ) noexcept
        {
            val_ ++;
            len_ --;
            return *this;
        }

        slice operator ++ (int) noexcept
        {
            slice tmp( val_, len_ );
            ++(*this);
            return tmp;
        }

    private:

        value_type  *val_ = nullptr;
        size_type    len_ = 0;
    };

}}

#endif // SLICE_H
