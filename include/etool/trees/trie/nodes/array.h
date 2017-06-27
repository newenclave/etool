#ifndef ETOOL_TREES_TRIE_NODES_ARRAY_H
#define ETOOL_TREES_TRIE_NODES_ARRAY_H

#include <vector>
#include <memory>
#include <algorithm>

namespace etool { namespace trees { namespace trie {

namespace nodes {

    template <typename KeyT, typename ValueT, typename Comp = std::less<KeyT> >
    class array {
    public:

        using key_type   = KeyT;
        using value_type = ValueT;
        using value_ptr  = std::unique_ptr<value_type>;

    private:

        using this_type  = array<key_type, value_type>;
        struct key_data {

            key_type    key;
            this_type   value;

            key_data( const key_data &other ) = delete;
            key_data& operator = ( const key_data &other ) = delete;

            key_data( const key_type &k )
                :key(k)
            { }

            key_data( key_data &&other )
                :key(std::move(other.key))
                ,value(std::move(other.value))
            { }

            key_data& operator = ( key_data &&other )
            {
                key   = std::move(other.key);
                value = std::move(other.value);
                return *this;
            }

            bool equal_keys( const key_type &rh )
            {
                return (!(key < rh)) && (!(rh < key));
            }

        };

        struct cmp {
            bool operator ( )( const key_data &d, const key_type &k ) const
            {
                const Comp compare;
                return compare( d.key, k );
            }
        };

        using container  = std::vector<key_data>;

    public:

        array *get( const key_type &k )
        {
            if( next_.empty( ) ) {
                return nullptr;
            }
            auto f = std::lower_bound( next_.begin( ), next_.end( ),
                                       k, cmp( ) );
            if( f != next_.end( ) ) {
                return (f->equal_keys( k ) ) ? &f->value : nullptr;
            } else {
                return nullptr;
            }
        }

        array *set( const key_type &k )
        {
            if( next_.empty( ) ) {
                next_.emplace_back( key_data(k) );
                return &next_.begin( )->value;
            } else {
                auto f = std::lower_bound(next_.begin( ), next_.end( ),
                                          k, cmp( ) );
                if( f != next_.end( ) && f->equal_keys( k ) ) {
                    return &f->value;
                } else {
                    auto nf = next_.emplace( f, key_data(k) );
                    return &nf->value;
                }
            }
        }

        void set_value( value_type val )
        {
            val_.reset( new value_type(std::move(val) ) );
        }

        value_type *value( )
        {
            return val_.get( );
        }

        const value_type *value( ) const
        {
            return val_.get( );
        }

    private:

        value_ptr  val_;
        container  next_;
    };
}

}}}

#endif // ETOOL_TREES_TRIE_NODES_ARRAY_H

