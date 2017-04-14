#ifndef ETOOL_DETAILS_CONTAINER_SLICE_H
#define ETOOL_DETAILS_CONTAINER_SLICE_H

#include  <utility>
#include  <algorithm>

namespace etool { namespace details {

    template <typename ItrT>
    struct container_slice {

        using iterator = ItrT;

        container_slice(                                      ) = default;
        container_slice( container_slice&&                    ) = default;
        container_slice( const container_slice&               ) = default;
        container_slice& operator = ( container_slice &&      ) = default;
        container_slice& operator = ( const container_slice & ) = default;

        container_slice( iterator b, iterator e )
            :data(std::move(b), std::move(e))
        { }

        iterator begin( ) const
        {
            return data.first;
        }

        iterator end( ) const
        {
            return data.second;
        }

        template <typename Distance>
        container_slice &operator += ( Distance len )
        {
            std::advance(data.first, len);
            return *this;
        }

        template <typename Distance>
        container_slice &operator -= ( Distance len )
        {
            std::advance(data.first, -len);
            return *this;
        }

        container_slice &operator ++ ( )
        {
            ++data.first;
            return *this;
        }

        container_slice operator ++ (int)
        {
            container_slice tmp( begin( ), end( ) );
            ++(*this);
            return tmp;
        }

    private:
        std::pair<iterator, iterator> data;
    };

}}

#endif // ETOOL_DETAILS_CONTAINER_SLICE_H
