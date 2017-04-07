
#ifndef ETOOL_TREES_TRIE_NODES_SIMPLE_UPTR_H
#define ETOOL_TREES_TRIE_NODES_SIMPLE_UPTR_H

#include <map>
#include <memory>

namespace etool { namespace trees { namespace trie {

namespace nodes {

    template <typename KeyT, typename ValueT>
    class simple_uptr {

    public:

        using key_type   = KeyT;
        using value_type = ValueT;

        using simple_uptr_ptr = std::unique_ptr<simple_uptr>;
        using nodes_map       = std::map<key_type, simple_uptr_ptr>;

        simple_uptr( )
            :final_(false)
        { }

        simple_uptr *get( const key_type &k )
        {
            auto f = next_.find( k );
            return (f == next_.end( )) ? nullptr : f->second.get( );
        }

        static
        simple_uptr_ptr create( )
        {
            return std::move(simple_uptr_ptr(new simple_uptr));
        }

        simple_uptr *add( const key_type &k )
        {
            auto f = next_.insert( std::make_pair( k, create( ) ) );
            return f.first->second.get( );
        }

        bool final( ) const
        {
            return final_;
        }

        void set_final( bool val )
        {
            final_ = val;
        }

        void set_value( value_type val )
        {
            val_ = std::move(val);
        }

        value_type &value( )
        {
            return val_;
        }

        const value_type &value( ) const
        {
            return val_;
        }

    private:

        bool        final_;
        value_type  val_;
        nodes_map   next_;

    };
}


}}}

#endif
