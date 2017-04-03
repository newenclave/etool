#ifndef ETOOL_OBSERVERS_BASE_H
#define ETOOL_OBSERVERS_BASE_H

#include <set>

#include <mutex>
#include <memory>

#include "etool/details/list.h"
#include "etool/observers/traits/simple.h"

#include "etool/observers/scoped-subscription.h"
#include "etool/observers/subscription.h"

namespace etool { namespace observers {

    template <typename SlotType, typename MutexType>
    class base {

        typedef base<SlotType, MutexType> this_type;

    public:

        typedef SlotType slot_traits;
        typedef typename SlotType::value_type  slot_type;

    private:

        typedef MutexType                     mutex_type;
        typedef std::lock_guard<mutex_type>   guard_type;

        struct impl {

            typedef base<SlotType, MutexType> parent_type;

            struct slot_info {
                slot_info( const slot_type &slot, size_t id )
                    :slot_(slot)
                    ,id_(id)
                { }
                slot_type slot_;
                size_t    id_;
            };

            /// NOT STL LIST!
            typedef details::list<slot_info>      list_type;
            typedef typename list_type::iterator  list_iterator;
            typedef std::set<size_t>              iterator_set;

            iterator_set        removed_;
            list_type           list_;
            list_type           added_;

            mutable mutex_type  list_lock_;
            mutable mutex_type  tmp_lock_;
            size_t              id_;

            impl( )
                :id_(1)
            { }

            ~impl( )
            {
                clear_unsafe( );
                //clear( ); // ??? hm
            }

            static
            list_iterator itr_erase( list_type &lst, list_iterator itr )
            {
                slot_traits::erase( itr->slot_ );
                return lst.erase( itr );
            }

            static
            list_iterator itr_rerase( list_type &lst, list_iterator itr )
            {
                slot_traits::erase( itr->slot_ );
                return lst.rerase( itr );
            }

            void unsubscribe( size_t itr )
            {
                guard_type lck(tmp_lock_);
                removed_.insert( itr );
                remove_by_index(added_, itr);
            }

            bool is_removed( size_t itr )
            {
                guard_type lck(tmp_lock_);
                return (removed_.erase( itr ) != 0);
            }

            static
            void remove_by_index( list_type &lst, size_t id )
            {
                if( lst.size( ) > 0 ) {

                    size_t min_id = lst.begin( )->id_;
                    size_t max_id = lst.rbegin( )->id_;

                    if( (id < min_id) || (id > max_id) ) {

                        return;

                    } else if( (id - min_id) < (max_id - id) ) {
                        /// CLOSE to begin!
                        list_iterator b(lst.begin( ));

                        for( ; b && (b->id_<id); ++b );

                        if( b && ( b->id_ == id ) ) {
                            itr_erase( lst, b );
                        }

                    } else {
                        /// CLOSE to end!
                        list_iterator b(lst.rbegin( ));

                        for( ; b && (id < b->id_); --b );

                        if( b && (b->id_ == id) ) {
                            itr_rerase( lst, b );
                        }
                    }
                }
            }

            void clear_removed( )
            {
                iterator_set tmp;
                {
                    guard_type lck(tmp_lock_);
                    tmp.swap( removed_ );
                }

                typename iterator_set::iterator b(tmp.begin( ));
                typename iterator_set::iterator e(tmp.end( ));

                list_iterator bl(list_.begin( ));

                for( ; (b!=e) && bl; ++b ) {
                    for( ; bl && (bl->id_ < *b); ++bl );
                    if( bl && (bl->id_ == *b) ) {
                        bl = itr_erase( list_, bl );
                    }
                }
            }

            void clear_unsafe( ) /// unsafe
            {
                list_iterator b = list_.begin( );
                while( b ) {
                    b = itr_erase( list_, b );
                }

                b = added_.begin( );
                while( b ) {
                    b = itr_erase( added_, b );
                }

                removed_.clear( );
            }

            void clear( )
            {
                guard_type l0(list_lock_);
                guard_type l1(tmp_lock_);
                clear_unsafe( );
            }

            void splice_added( )
            {
                guard_type lck(tmp_lock_);
                list_.splice_back( added_ );
            }

            size_t connect( slot_type call )
            {
                guard_type l(tmp_lock_);
                size_t next = id_++;
                added_.push_back( slot_info(call, next) );
                return next;
            }

        };

        typedef std::shared_ptr<impl>   impl_sptr;
        typedef std::weak_ptr<impl>     impl_wptr;

        struct unsubscriber: subscription::unsubscriber {

            typedef base<SlotType, MutexType> base_type;

            typedef typename base_type::impl_sptr impl_sptr;
            typedef typename base_type::impl_wptr impl_wptr;

            unsubscriber( impl_wptr p, size_t k )
                :parent(p)
                ,key(k)
            { }

            std::uintptr_t data( )
            {
                return reinterpret_cast<std::uintptr_t>(parent.lock( ).get( ));
            }

            void run( )
            {
                impl_sptr lock(parent.lock( ));
                if( lock ) {
                    lock->unsubscribe( key );
                }
            }

            impl_wptr parent;
            size_t    key;
        };

    public:

        typedef observers::subscription subscription;
        typedef observers::scoped_subscription scoped_subscription;

        friend class observers::subscription;
        friend class observers::scoped_subscription;

        typedef subscription connection;

        base( )
            :impl_(std::make_shared<impl>( ))
        { }

        base( const base & )              = delete;
        base& operator = ( const base & ) = delete;
        base( base &&o )
        {
            impl_.swap( o.impl_ );
            o.impl_ = std::make_shared<impl>( );
        }

        base & operator = ( base &&o )
        {
            impl_.swap( o.impl_ );
            o.impl_ = std::make_shared<impl>( );
            return *this;
        }

        ~base( ) = default;

        subscription subscribe( slot_type call )
        {
            size_t next = impl_->connect( call );

            subscription::unsubscriber_sptr us =
                    std::make_shared<unsubscriber>(impl_wptr(impl_), next);

            return subscription( us );
        }

        subscription connect( slot_type call )
        {
            return subscribe( call );
        }

        void unsubscribe( subscription &cc )
        {
            cc.unsubscribe( );
        }

        void disconnect( subscription &cc )
        {
            unsubscribe( cc );
        }

        void unsubscribe_all(  )
        {
            impl_->clear( );
        }

        void disconnect_all_slots(  )
        {
            unsubscribe_all( );
        }

        template <typename ...Args>
        void operator ( ) ( const Args& ...args )
        {
            guard_type l(impl_->list_lock_);
            impl_->splice_added( );
            typename impl::list_iterator b(impl_->list_.begin( ));
            while( b ) {
                if( impl_->is_removed( b->id_ ) ) {
                    b = impl::itr_erase( impl_->list_, b );
                } else {
                    slot_traits::exec( b->slot_, args... );
                    if( slot_traits::expired( b->slot_ ) ) {
                        b = impl::itr_erase( impl_->list_, b );
                    } else {
                        ++b;
                    }
                }
            }
            impl_->clear_removed( );
        }

    private:
        impl_sptr impl_;
    };

}}

#endif // SIMPLE_H

