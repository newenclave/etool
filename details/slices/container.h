#ifndef ETOOL_DETAILS_SLICES_CONTAINER_H
#define ETOOL_DETAILS_SLICES_CONTAINER_H

#include  <utility>
#include  <algorithm>

namespace etool { namespace details {

    template <typename ItrT>
    struct container {

        using iterator = ItrT;

        container(                                      ) = default;
        container( container&&                    ) = default;
        container( const container&               ) = default;
        container& operator = ( container &&      ) = default;
        container& operator = ( const container & ) = default;

        container( iterator b, iterator e )
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
        container &operator += ( Distance len )
        {
            std::advance(data.first, len);
            return *this;
        }

        template <typename Distance>
        container &operator -= ( Distance len )
        {
            std::advance(data.first, -len);
            return *this;
        }

        container &operator ++ ( )
        {
            ++data.first;
            return *this;
        }

        container operator ++ (int)
        {
            container tmp( begin( ), end( ) );
            ++(*this);
            return tmp;
        }

    private:
        std::pair<iterator, iterator> data;
    };

}}

#endif // ETOOL_DETAILS_CONTAINER_SLICE_H
