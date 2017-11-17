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

        struct special {
            static const std::size_t unsubscribe_all = 1;
        };

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
            std::size_t         id_ = 10;
            std::uint32_t       current_enter_ = 0;

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

            bool check_clean( )
            {
                guard_type lck(tmp_lock_);
                if ( removed_.erase(special::unsubscribe_all) != 0 ) {
                    clear_added_unsafe( );
                    clear_main_unsafe( );
                    removed_.clear( );
                    return true;
                }
                return false;
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
                    for (; bl && (bl->id_ < *b); ++bl)
                    { }
                    if( bl && (bl->id_ == *b) ) {
                        bl = itr_erase( list_, bl );
                    }
                }
            }

            void clear_added_unsafe( )
            {
                list_iterator b = added_.begin();
                while (b) {
                    b = itr_erase(added_, b);
                }
            }

            void clear_main_unsafe( )
            {
                list_iterator b = list_.begin();
                while( b ) {
                    b = itr_erase( list_, b );
                }
            }

            void clear_unsafe( ) /// unsafe
            {
                clear_main_unsafe( );
                clear_added_unsafe( );
                removed_.clear( );
            }

            void clear( )
            {
                guard_type lock(tmp_lock_);
                removed_.insert(special::unsubscribe_all);
                clear_added_unsafe( );
            }

            void splice_added( )
            {
                guard_type lck(tmp_lock_);
                list_.splice_back( added_ );
            }

            std::size_t next_id()
            {
                return id_ += 2;
            }

            std::size_t connect( slot_type call )
            {
                guard_type l(tmp_lock_);
                std::size_t next = next_id();
                added_.emplace_back( slot_info(call, next) );
                return next;
            }

            template <typename ...Args>
            void call( Args&& ...args )
            {
                static const std::thread::id empty_thread;
                guard_type l(list_lock_);

                if (++current_enter_ == 1) {
                    if( check_clean( ) ) {
                        return;
                    }
                    splice_added();
                }

                typename impl::list_iterator b(list_.begin( ));
                while( b ) {
                    if( is_removed( b->id_ ) ) {
                        b = impl::itr_erase( list_, b );
                    } else {
                        if( !slot_traits::expired( b->slot_ ) ) {
                            slot_traits::exec( b->slot_,
                                               std::forward<Args>(args)... );
                            ++b;
                        } else {
                            b = impl::itr_erase( list_, b );
                        }
                    }
                }
                if (--current_enter_ == 0) {
                    clear_removed();
                }
            }
        };

        typedef std::shared_ptr<impl>   impl_sptr;
        typedef std::weak_ptr<impl>     impl_wptr;

        struct unsubscriber: subscription::unsubscriber {

            typedef base<SlotType, MutexType> base_type;

            typedef typename base_type::impl_sptr impl_sptr;
            typedef typename base_type::impl_wptr impl_wptr;

            unsubscriber( impl_wptr p, std::size_t k )
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

            impl_wptr   parent;
            std::size_t key;
        };

        impl &get_impl( )
        {
            return *impl_;
        }

        impl_wptr get_impl_wptr( )
        {
            return impl_;
        }

        const impl &get_impl( ) const
        {
            return *impl_;
        }

    public:

        using subscription        = observers::subscription;
        using scoped_subscription = observers::scoped_subscription;

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
       }

        base & operator = ( base &&o )
        {
            impl_.swap( o.impl_ );
            return *this;
        }

        ~base( ) = default;

        subscription subscribe( slot_type call )
        {
            size_t next = get_impl().connect( call );

            subscription::unsubscriber_sptr us =
                    std::make_shared<unsubscriber>(get_impl_wptr( ), next);

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
            get_impl().clear( );
        }

        void disconnect_all_slots(  )
        {
            unsubscribe_all( );
        }

        template <typename ...Args>
        void operator ( ) ( Args&& ...args )
        {
            get_impl( ).call( std::forward<Args>( args )... );
        }

    private:
        impl_sptr impl_;
    };

}}

#endif // SIMPLE_H

