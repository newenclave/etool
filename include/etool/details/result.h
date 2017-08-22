#ifndef ETOOL_DETAILS_RESULT_H
#define ETOOL_DETAILS_RESULT_H

#include <sstream>
#include <memory>

namespace etool { namespace details {

    namespace traits {

        template <typename T>
        struct value {

            typedef T value_type;

            static
            void copy( value_type &v, const value_type &from )
            {
                v = from;
            }

            static
            value_type create(  )
            {
                return value_type( );
            }

            template <typename ...Args>
            static
            value_type create( Args&&...args )
            {
                return value_type(std::forward<Args>( args )... );
            }

            static
            void destroy( value_type & )
            { }

            static
            value_type &&move( value_type &v )
            {
                return std::move(v);
            }

            static
            value_type &get( value_type &v )
            {
                return v;
            }

            static
            const value_type &get( value_type const &v )
            {
                return v;
            }

        };

        template <typename T>
        struct shared_ptr {

            typedef std::shared_ptr<T> value_type;

            static
            void copy( value_type &v, const value_type &from )
            {
                v = from;
            }

            static
            value_type create(  )
            {
                return std::make_shared<T>( );
            }

            template <typename ...Args>
            static
            value_type create( Args&&...args )
            {
                return std::make_shared<T>( std::forward<Args>( args )... );
            }

            static
            void destroy( value_type & )
            { }

            static
            T &&move( value_type &v )
            {
                return std::move(*v);
            }

            static
            T &get( value_type &v )
            {
                return *v;
            }

            static
            const T &get( value_type const &v )
            {
                return *v;
            }
        };
    }

    template <typename T, typename E,
              typename Trait = traits::value<T> >
    class result {

    public:

        typedef T value_type;
        typedef E error_type;

        struct value_arg {


            template <typename ...Args>
            explicit
            value_arg( Args&& ...args )
                :val_(Trait::create(std::forward<Args>(args)...))
            { }

            value_type &&move( )
            {
                return std::move(val_);
            }

        private:
            value_type val_;
        };

        struct error_arg {

            template <typename ...Args>
            explicit
            error_arg( Args&& ...args )
                :val_(std::forward<Args>(args)...)
            { }

            error_type &&move( )
            {
                return std::move(val_);
            }

        private:
            error_type val_;
        };

        result( )
            :value_(Trait::create( ))
            ,failed_(false)
        { }

        result( const result &other )
            :value_(Trait::create(other))
            ,error_(other.error_)
            ,failed_(other.failed_)
        { }

        result( result &&other )
            :value_(std::move(other.value_))
            ,error_(std::move(other.error_))
            ,failed_(other.failed_)
        { }

        explicit
        result( value_arg &&val )
            :value_(val.move( ))
            ,error_(error_type( ))
            ,failed_(false)
        { }

        explicit
        result( error_arg &&val )
            :error_(val.move( ))
            ,failed_(true)
        { }

        result &operator = ( const result &other )
        {
            failed_ = other.failed_;
            error_  = other.error_;
            Trait::copy( value_, other.value_ );
            return *this;
        }

        result &operator = ( result &&other )
        {
            failed_ = other.failed_;
            error_  = std::move(other.error_);
            value_  = std::move(other.value_);
            return *this;
        }

        ~result( )
        {
            Trait::destroy( value_ );
        }

        template <typename ...Args>
        static
        result ok( Args&& ...args )
        {
            return result( value_arg(std::forward<Args>(args)...) );
        }

        template <typename ...Args>
        static
        result fail( Args&& ...args )
        {
            return result( error_arg(std::forward<Args>(args)...) );
        }

        void swap( result &other )
        {
            std::swap( value_,  other.value_ );
            std::swap( error_,  other.error_ );
            std::swap( failed_, other.failed_ );
        }

        value_type &operator *( )
        {
            return Trait::get(value_);
        }

        const value_type &operator *( ) const
        {
            return Trait::get(value_);
        }

        value_type *operator -> ( )
        {
            return &Trait::get(value_);
        }

        const value_type *operator -> ( ) const
        {
            return &Trait::get(value_);
        }

        operator bool ( ) const
        {
            return !failed_;
        }

        const error_type &error( ) const
        {
            return error_;
        }

        error_type &error( )
        {
            return error_;
        }

        value_type &&move( )
        {
            return Trait::move(value_);
        }

    private:

        typename Trait::value_type value_;
        error_type                 error_;
        bool                       failed_;
    };

    template <typename T, typename E>
    inline
    std::ostream & operator << ( std::ostream &o, const result<T, E> &res )
    {
        if( res ) {
            o << "Ok: "   << *res;
        } else {
            o << "Fail: " <<  res.error( );
        }
        return o;
    }

}}

#endif // RESULT_H
