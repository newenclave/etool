#ifndef ETOOL_DELAYED_QUEUE_BASE_H
#define ETOOL_DELAYED_QUEUE_BASE_H

#include <memory>
#include <functional>
#include <chrono>
#include <mutex>
#include <queue>
#include <vector>
#include <map>
#include <set>
#include <atomic>
#include <algorithm>
#include <condition_variable>

namespace etool { namespace queues { namespace delayed {


    class base {

        using duration_resolution = std::chrono::nanoseconds;
        static std::uint64_t now_ticks()
        {
            using namespace std::chrono;
            auto now = high_resolution_clock::now().time_since_epoch();
            return duration_cast<duration_resolution>(now).count();
        }

        using task_token_type = std::size_t;
        struct delayed_task_info
        {
            delayed_task_info(task_token_type id, std::uint64_t expires)
                :id_(id)
                ,expires_(expires)
            { }

            delayed_task_info() = default;
            delayed_task_info(delayed_task_info &&) = default;
            delayed_task_info &operator = (delayed_task_info &&) = default;
            delayed_task_info(const delayed_task_info &) = default;
            delayed_task_info &operator = (const delayed_task_info &) = default;

            operator bool( ) const
            {
                return (expires_ != 0) || (id_ != 0);
            }

            static
            bool heap_compare( const delayed_task_info &lh,
                               const delayed_task_info &rh )
            {
                return rh.expires_ < lh.expires_;
            }

            task_token_type id_ = 0;
            std::uint64_t expires_ = 0;
        };

        struct impl;

    public:

        using task_type = std::function<void(void)>;
        using delayed_task_type = std::function<void(bool)>;

        base( ) = default;
        base( base &&) = default;
        base &operator = ( base && ) = default;
        base( const base & ) = delete;
        base &operator = ( const base & ) = delete;

        ~base( )
        {
            try {
                cancel_all_delayed();
            } catch (...) {
            }
        }

        class delayed_task
        {
            friend class base;
        public:

            delayed_task(base::impl &queue, const delayed_task_info &task)
                :queue_(&queue)
                ,info_(task)
            {}

            delayed_task() = default;
            delayed_task(const delayed_task&) = default;
            delayed_task &operator = (const delayed_task &) = default;

            delayed_task(delayed_task && other)
            {
                swap(other);
            }

            delayed_task &operator = (delayed_task && other)
            {
                swap(other);
                return *this;
            }

            ~delayed_task()
            { }

            void cancel()
            {
                if (queue_) {
                    queue_->cancel_delayed_task(*this);
                }
            }

            bool is_active() const
            {
                if (queue_) {
                    return queue_->is_active_id(info_.id_);
                }
                return false;
            }

            operator bool() const
            {
                return (queue_ != nullptr) && (info_.operator bool());
            }

            void swap(delayed_task &other)
            {
                std::swap(queue_, other.queue_);
                std::swap(info_, other.info_);
            }

        private:
            base::impl *queue_ = nullptr;
            delayed_task_info info_;
        };

    private:

        /*
        *   The wrapper class for the tasks.
        *   It just is created for the future.
        *   For example use std::future std::promise
        */
        template <typename TaskT>
        struct templ_task_wrapper
        {
            templ_task_wrapper(TaskT task) :
                m_task(std::move(task))
            {}

            templ_task_wrapper(std::nullptr_t)
            {}

            templ_task_wrapper() = default;
            templ_task_wrapper(templ_task_wrapper &&other)
            {
                swap(other);
            }

            templ_task_wrapper& operator = (templ_task_wrapper &&other)
            {
                swap(other);
                return *this;
            }

            templ_task_wrapper(const templ_task_wrapper &) = default;
            templ_task_wrapper&
            operator = (const templ_task_wrapper &) = default;

            operator bool() const
            {
                return m_task.operator bool();
            }

            template <typename ...Args>
            void operator ()(Args&& ...args) const
            {
                m_task(std::forward<decltype(args)>(args)...);
            }

            void swap(templ_task_wrapper &other)
            {
                std::swap(m_task, other.m_task);
            }

        private:
            TaskT m_task;
        };

        using mutex_type = std::mutex;
        using delayed_task_wrapper = templ_task_wrapper<delayed_task_type>;
        using task_wrapper = templ_task_wrapper<task_type>;

        using delayed_task_map = std::map<task_token_type, delayed_task_wrapper>;
        using delayed_task_set = std::set<task_token_type>;
        using task_queue_type = std::queue<task_wrapper>;

        /*
        *   Why impl?
        *   For any future purposes.
        *   It will be more easier to make the Impl some smart pointer
        *       (by traits for example)
        */
        struct impl
        {
            impl() = default;
            impl(impl &&) = default;
            impl &operator = (impl &&) = default;
            impl(const impl &) = default;
            impl &operator = (const impl &) = default;

            bool is_active() const
            {
                std::unique_lock<mutex_type> lock(work_lock_);
                return enabled_;
            }

            void stop(bool waitRegular)
            {
                task_queue_type tmp;
                {
                    std::unique_lock<mutex_type> lock(work_lock_);
                    enabled_ = false;
                    task_queue_.swap(tmp);
                    work_cond_.notify_all();
                }
                cancel_all_delayed_tasks();
                if (waitRegular) {
                    while (!tmp.empty()) {
                        tmp.front()();
                        tmp.pop();
                    }
                }
            }

            void cancel_all_delayed_tasks()
            {
                delayed_task_map tmp;
                {
                    std::lock_guard<mutex_type> lock(active_lock_);
                    tmp.swap(in_wait_);
                }

                for (auto &t : tmp) {
                    t.second(true);
                }
            }

            void reset()
            {
                std::unique_lock<mutex_type> lock(work_lock_);
                enabled_ = true;
            }

            std::size_t tasks_size() const
            {
                return task_queue_.size();
            }

            std::size_t delayed_size() const
            {
                return in_wait_.size();
            }

            void cancel_delayed_task(const delayed_task &task)
            {
                if (task.queue_ == this) {
                    cancel_delayed_task(task.info_.id_);
                }
                //else
                //should we raise exception here?....
            }

            bool cancel_delayed_task(task_token_type id)
            {
                /*
                *   First of all we are trying to remove the task
                *   with desired id from the wait list and the process set.
                *   If there is any task there we just move it to tmp objects
                *   and post another Regular Task.
                *   The regular task triggers any "wait" on the
                *   condition variable and the task with "cancelled"
                *   state is executed
                *   So we can cancel any task even with "zero period".
                */
                delayed_task_wrapper task = nullptr;
                {
                    std::lock_guard<mutex_type> lock(active_lock_);
                    auto f = in_wait_.find(id);
                    if (f == in_wait_.end()) {
                        if (in_process_.erase(id) > 0) {
                            work_cond_.notify_all();
                        }
                        return false;
                    }
                    task = std::move(f->second);
                    in_wait_.erase(f);
                }

                /*
                *   Cancel is also a regular task for the queue
                */
                return post_task([task]() mutable {
                    task(true);
                });
            }

            void activate_delayed_task( task_token_type id,
                                      delayed_task_wrapper task )
            {
                std::lock_guard<mutex_type> lock(active_lock_);
                in_wait_.emplace(id, std::move(task));
                in_process_.erase(id);
            }

            template <typename Duration>
            delayed_task post_delayed_task( Duration dur,
                                            delayed_task_type handler )
            {
                using namespace std::chrono;
                /*
                *   Unable to post to inactive queue
                */
                if (!is_active()) {
                    return delayed_task{};
                }

                auto id = next_id( );
                delayed_task_info taskInfo(id,
                        now_ticks() +
                            duration_cast<duration_resolution>(dur).count());

                delayed_task res(*this, taskInfo);
                activate_delayed_task(id, std::move(handler));

                /*
                *   Posting task makes Delayed and Regular task more competitive
                *   Also it makes the logic of WaitNext more clear
                */
                post_task([this, taskInfo]() {
                    std::lock_guard<mutex_type> lock(work_lock_);
                    register_delayed_task(std::move(taskInfo));
                    work_cond_.notify_one();
                });
                return res;
            }

            template <typename Duration>
            delayed_task post_delayed_task(Duration dur, task_type handler)
            {
                return post_delayed_task(dur, [handler](bool cancelled) {
                    if (!cancelled) {
                        handler();
                    }
                });
            }

            template <typename Handler>
            bool post_task(Handler &&handler)
            {
                /*
                *   Unable to post to inactive queue
                */
                if (!is_active()) {
                    return false;
                }
                std::unique_lock<mutex_type> lock(work_lock_);
                task_queue_.emplace(std::forward<decltype(handler)>(handler));
                work_cond_.notify_one();
                return true;
            }

            bool post_stop(bool wait_regular_tasks = false)
            {
                return post_task([this, wait_regular_tasks]() {
                    stop(wait_regular_tasks);
                });
            }

            std::size_t run( )
            {
                std::size_t count = 0;
                while (auto next = wait_next()) {
                    ++count;
                    next();
                }
                return count;
            }

            std::size_t run(std::size_t init)
            {
                std::size_t count = 0;
                while (init > count) {
                    if (auto next = wait_next()) {
                        ++count;
                        next();
                    } else {
                        break;
                    }
                }
                return count;
            }

            /*
            *   Returns a deleyed task by its token id
            *   if the task exists in the wait map "m_inWait".
            *   Also the call insert the Id to the process set "m_inProcess"
            */
            delayed_task_wrapper pop_if_active(task_token_type id)
            {
                std::lock_guard<mutex_type> lock(active_lock_);
                auto f = in_wait_.find(id);
                if (f != in_wait_.end())
                {
                    auto task = std::move(f->second);
                    in_process_.insert(f->first);
                    in_wait_.erase(f);
                    return task;
                }
                return nullptr;
            }

            /*
            *   Checks if the task presents in the wait map or the process set
            *   In this case it can be cancelled.
            */
            bool is_active_id(task_token_type id) const
            {
                std::lock_guard<mutex_type> lock(active_lock_);
                return ( in_wait_.find(id) != in_wait_.end( ) )
                    || ( in_process_.find(id) != in_process_.end( ) );
            }

            bool is_in_process( task_token_type id ) const
            {
                std::lock_guard<mutex_type> lock(active_lock_);
                return in_process_.find(id) != in_process_.end();
            }

            /*
            *   Removes delayed task token from the process set
            */
            bool remove_from_process( task_token_type id )
            {
                std::lock_guard<mutex_type> lock(active_lock_);
                return in_process_.erase(id) > 0;
            }

            task_token_type next_id( )
            {
                return m_id++;
            }

            /*
            *   Obtains next regulag task if the queue is not empty
            *   returns empty object if no task was found.
            *   Must be called under m_workLock mutex protection
            */
            task_wrapper pop_task( )
            {
                // call under m_workLock mutex
                if ( !task_queue_.empty( ) ) {
                    auto task = std::move( task_queue_.front( ) );
                    task_queue_.pop( );
                    return task;
                }
                return nullptr;
            }

            /*
            *   Pushes a task info to the delayed heap.
            *   Must be called under m_workLock mutex protection
            */
            void register_delayed_task( delayed_task_info value )
            {
                // call under m_workLock mutex
                delayed_.emplace_back(std::move(value));
                std::push_heap( delayed_.begin( ), delayed_.end( ),
                                &delayed_task_info::heap_compare);
            }

            /*
            *   Returns next delayed task information if it exists.
            *   Must be called under m_workLock mutex protection
            */
            delayed_task_info pop_delayed_task_info( )
            {
                // call under m_workLock mutex
                if ( !delayed_.empty( ) ) {
                    std::pop_heap( delayed_.begin( ), delayed_.end( ),
                                   &delayed_task_info::heap_compare );
                    auto res = std::move( delayed_.back());
                    delayed_.pop_back();
                    return res;
                }
                return delayed_task_info { };
            }

            /*
                Obtains next task from the queue
                First of all it checks if there are some delayed task here
                And if there are not, just waits for a regular task
            */
            task_type wait_next( )
            {
                std::unique_lock<mutex_type> lock(work_lock_);

                while (enabled_) {
                    auto next = pop_delayed_task_info();
                    if (!next) {
                        work_cond_.wait(lock, [this]( ) {
                            return !enabled_ || !task_queue_.empty();
                        });

                        if( auto task = pop_task( ) ) {
                            return task;
                        }
                    } else {
                        auto next_delay_task = pop_if_active( next.id_ );
                        if ( !next_delay_task ) {
                            continue;
                        }

                        auto now = now_ticks();

                        if ( next.expires_ > now ) {
                            auto timeout = duration_resolution(next.expires_
                                                               - now);
                            auto wait_res = work_cond_.wait_for(lock, timeout,
                                [this, &next]( ) {
                                    return !enabled_ ||
                                        !task_queue_.empty( ) ||
                                        !is_in_process( next.id_ );
                                });

                            if( !is_in_process( next.id_ )) {
                                return [next_delay_task]( ) {
                                    next_delay_task(true);
                                };
                            }

                            /*
                            *   Here check if some regular task is in the queue
                            *   So it can be executed first.
                            *   Also we have to post the delayed task
                            *       (we obtained before) back to the wait map.
                            */
                            task_wrapper task;
                            if( wait_res && (task = pop_task( ) ) ) {
                                activate_delayed_task( next.id_,
                                             std::move( next_delay_task ) );
                                register_delayed_task( next );
                                return task;
                            }
                        }
                        bool is_in_pocess = remove_from_process(next.id_);
                        return [next_delay_task, is_in_pocess]( ) {
                            next_delay_task(!is_in_pocess);
                        };
                    }
                }
                return nullptr;
            }

            //////////////// values ////////////////
            bool enabled_ = true;
            std::atomic<task_token_type> m_id{ 100 };

            std::vector<delayed_task_info> delayed_;
            task_queue_type task_queue_;
            std::condition_variable work_cond_;
            mutable mutex_type work_lock_;

            /*
            *   The wait map and the process set are used for control
            *       the execution of delayed call.
            *   As far as delayed task can be cancelled, we have to know
            *   where is the task now.
            *   if the task is in the "m_inWait" map,
            *       so none of the threads has it.
            *   if the task is in the "m_inProcess" set, some of the thread has
            *       activated the task and now is waiting the time of the task.
            *   Both map and set should be processed under "m_activeLock" mutex
            */
            delayed_task_map in_wait_;
            delayed_task_set in_process_;
            mutable mutex_type active_lock_;
        };

        impl &get_impl( )
        {
            return impl_;
        }

        const impl &get_impl( ) const
        {
            return impl_;
        }

    public:

        /*
        *   An object of this class is used as a result of Wrap call.
        *   It holds the reference to the Delayed object and a task.
        *   It will post the task to the queue whan it is invoked.
        */
        template <typename TaskT>
        struct post_task_wrapper
        {
            post_task_wrapper(base &queue, TaskT task) :
                queue_(queue),
                task_(std::move(task))
            {}

            post_task_wrapper(post_task_wrapper &&other) = default;
            post_task_wrapper(const post_task_wrapper &) = default;
            post_task_wrapper& operator = (post_task_wrapper &&) = default;
            post_task_wrapper& operator = (const post_task_wrapper &) = default;

            /*
            *   Warning! std::bind loses references.
            *   Use pointers or std::ref
            */
            template <typename ...Args>
            void operator ( )(Args&& ...args) const
            {
                auto t = std::bind( task_,
                                    std::forward<decltype(args)>(args)... );
                queue_.post_task( t );
            }

        private:
            base &queue_;
            TaskT task_;
        };

        class scoped_delayed_task
        {

        public:

            scoped_delayed_task() = default;

            scoped_delayed_task(delayed_task task)
                :task_(std::move(task))
            { }

            scoped_delayed_task(scoped_delayed_task &&other)
                :task_(std::move(other.task_))
            { }

            scoped_delayed_task& operator = (scoped_delayed_task &&other)
            {
                task_.swap(other.task_);
                return *this;
            }

            scoped_delayed_task& operator = (delayed_task &&other)
            {
                task_.swap(other);
                return *this;
            }

            scoped_delayed_task& operator = (const delayed_task &other)
            {
                scoped_delayed_task tmp(other);
                swap(tmp);
                return *this;
            }

            scoped_delayed_task(const scoped_delayed_task &other) = delete;
            scoped_delayed_task&
            operator = (const scoped_delayed_task &other) = delete;

            ~scoped_delayed_task( )
            {
                cancel( );
            }

            void cancel( )
            {
                task_.cancel( );
            }

            bool is_active( ) const
            {
                return task_.is_active( );
            }

            void swap(scoped_delayed_task &other)
            {
                task_.swap(other.task_);
            }

            operator bool() const
            {
                return task_.operator bool();
            }

        private:
            delayed_task task_;
        };

        template <typename Duration>
        delayed_task post_delayed_task(Duration dur, delayed_task_type handler)
        {
            return get_impl().post_delayed_task(dur, std::move(handler));
        }

        /*
        *   PostDelayedTask warpper for a task with no parameters
        *   This call also returns DelayedTask object
        *       and the task can be cancelled
        *   But the call will not be made in the case of the cancellation.
        */
        template <typename Duration>
        delayed_task post_delayed_task(Duration dur, task_type handler)
        {
            return get_impl().post_delayed_task(dur, std::move(handler));
        }

        /*
        *   Posts a regular task to the queue.
        *   Returns true if the task was successfully posted
        *       and false if the queue is already stopped
        */
        template <typename Handler>
        bool post_task(Handler &&task)
        {
            return get_impl( ).post_task(std::forward<decltype(task)>(task));
        }

        /*
        *   Posts the stop signal to the queue;
        */
        bool post_stop(bool wait_all_regular_tasks = false)
        {
            return get_impl( ).post_stop( wait_all_regular_tasks );
        }

        /*
        *   Starts the process of execution of tasks.
        *   Objains next task and execute it.
        *   @result: executed tasks count.
        */
        std::size_t run( )
        {
            return get_impl( ).run( );
        }

        std::size_t run(std::size_t hint)
        {
            return get_impl().run(hint);
        }

        /*
        *  Stops the queue.
        *  All active thread with "Run" should be notified and stopped.
        *  WaitNext in this case returns nullptr as next result
        */
        void stop(bool wait_all_regular_tasks = false)
        {
            get_impl( ).stop( wait_all_regular_tasks );
        }

        void Reset( )
        {
            get_impl( ).reset( );
        }

        std::size_t tasks_size( ) const
        {
            return get_impl( ).tasks_size( );
        }

        std::size_t delayed_size( ) const
        {
            return get_impl( ).delayed_size( );
        }

        bool is_active( ) const
        {
            return get_impl( ).is_active( );
        }

        void cancel_delayed_task( const delayed_task &task )
        {
            get_impl( ).cancel_delayed_task(task);
        }

        /*
        *   This function is used to create a new handler function object that,
        *   when invoked, will automatically pass the wrapped handler
        *   to the queue.
        */
        template <typename TaskT>
        post_task_wrapper<TaskT> wrap(TaskT &&task)
        {
            return post_task_wrapper<TaskT>(*this,
                                     std::forward<decltype(task)>(task));
        }

    private:

        // TODO
        //  what should we do with exceptions?...
        //
        void cancel_all_delayed()
        {
            get_impl().cancel_all_delayed_tasks();
        }

        /*
        *   Cancel a task with desired id.
        */
        bool cancel_delayed_task(task_token_type id)
        {
            return get_impl().cancel_delayed_task(id);
        }

    private:

        impl impl_;
    };

}}}

#endif // BASE_H
