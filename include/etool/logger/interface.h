#ifndef ETOOL_LOGGER_INTERFACE_H
#define ETOOL_LOGGER_INTERFACE_H

#include <sstream>
#include "etool/details/dummy_mutex.h"

namespace etool { namespace logger {

    class interface {

        struct string_accumulator {

            string_accumulator( interface *parent, int lev, std::string name )
                :parent_(parent)
                ,level_(lev)
                ,name_(name)
                ,act_(true)
            { }

            string_accumulator( string_accumulator & ) = delete;
            string_accumulator& operator = ( string_accumulator & ) = delete;

            string_accumulator( string_accumulator &&other )
                :parent_(other.parent_)
                ,level_(other.level_)
                ,name_(std::move(other.name_))
                ,act_(true)
            {
                other.act_ = false;
            }

            string_accumulator& operator = ( string_accumulator &&other )
            {
                swap( other );
                return *this;
            }

            ~string_accumulator( )
            {
                if( act_ && ( level_ <= parent_->level_ ) ) {
                    parent_->send_data( level_, name_, std::move(oss_.str( )) );
                }
            }

            template<typename T>
            string_accumulator &operator << ( const T &data )
            {
                if( level_ <= parent_->level_ ) {
                    oss_ << data;
                }
                return *this;
            }

            void swap( string_accumulator &other ) noexcept
            {
                std::swap( parent_, other.parent_ );
                std::swap( level_,  other.level_  );
                std::swap( act_,    other.act_    );

                name_.swap( other.name_ );
                oss_.swap ( other.oss_  );
            }

            interface              *parent_;
            int                     level_;
            std::string             name_;
            std::ostringstream      oss_;
            bool                    act_;
        };

    public:

        virtual
        void send_data( int lev, const std::string &name,
                        std::string data ) = 0;

        string_accumulator operator ( )( int lev, const std::string &name )
        {
            string_accumulator res( this, lev, name );
            return res;
        }

        string_accumulator operator ( )( int lev )
        {
            string_accumulator res( this, lev, "" );
            return res;
        }

        string_accumulator operator ( )( const std::string &name )
        {
            string_accumulator res( this, level_, name );
            return res;
        }

        string_accumulator operator ( )( )
        {
            string_accumulator res( this, level_, "" );
            return res;
        }

        interface( int lev )
            :level_(lev)
        { }

        virtual ~interface( )
        { }

        int get_level( ) const
        {
            return level_;
        }

        void set_level( int lev )
        {
            level_ = lev;
        }

    private:

        int level_;

    };


} }

#endif // INTERFACE_H
