#ifndef ETOOL_SLICES_MEMORY_H
#define ETOOL_SLICES_MEMORY_H

#include <cstdint>

namespace etool { namespace slices {

    template <typename ValueType, typename SizeType = std::size_t>
    class memory {
    public:

        using value_type     = ValueType;
        using size_type      = SizeType;
        using pointer_type   = value_type *;
        using reference_type = value_type &;

        memory( value_type *value, size_type len )
            :val_(value)
            ,len_(len)
        { }

        memory( value_type *begin, value_type *end )
            :val_(begin)
            ,len_(static_cast<size_type>(end - begin))
        { }

        memory             (                )  = default;
        memory             ( const memory & )  = default;
        memory             ( memory &&      )  = default;
        memory &operator = ( memory &&      )  = default;
        memory &operator = ( const memory & )  = default;

        template <typename U, typename S = size_type>
        memory<U, S> rebind( )
        {
            S new_size = (sizeof(value_type) * len_) / sizeof(U);
            memory<U, S> tmp( reinterpret_cast<U *>(val_), new_size );
            return tmp;
        }

        template <typename U, typename S = size_type>
        static
        memory bind( U *data, S len )
        {
            memory<U, S> tmp( data, len );
            return tmp.template rebind<value_type, size_type>( );
        }

        template <typename U, typename S = size_type>
        static
        memory bind( memory<U, S> other )
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

        memory &operator += ( size_type len ) noexcept
        {
            val_ += len;
            len_ -= len;
            return *this;
        }

        memory &operator -= ( size_type len ) noexcept
        {
            val_ -= len;
            len_ += len;
            return *this;
        }

        memory &operator ++ ( ) noexcept
        {
            val_ ++;
            len_ --;
            return *this;
        }

        memory operator ++ (int) noexcept
        {
            memory tmp( val_, len_ );
            ++(*this);
            return tmp;
        }

        memory &operator -- ( ) noexcept
        {
            val_ --;
            len_ ++;
            return *this;
        }

        memory operator -- (int) noexcept
        {
            memory tmp( val_, len_ );
            --(*this);
            return tmp;
        }

    private:

        value_type  *val_ = nullptr;
        size_type    len_ = 0;
    };

}}

#endif // SLICE_H
