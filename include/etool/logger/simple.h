#ifndef ETOOL_LOGGER_SIMPLE_H
#define ETOOL_LOGGER_SIMPLE_H

#include <ctime>
#include <memory>
#include <thread>

#include "etool/logger/interface.h"
#include "etool/observers/define.h"
#include "etool/queues/condition/base.h"

#include "etool/cache/none.h"
#include "etool/cache/simple.h"

namespace etool { namespace logger {

    class simple : public etool::logger::interface {

    public:
        using void_call = std::function<void()>;

        struct log_time {

            std::time_t seconds; // now in seconds
            std::uint64_t microsec; // now % 1000000

            log_time(std::time_t w, std::uint64_t m)
                : seconds(w)
                , microsec(m)
            {
            }

            log_time()
                : seconds(std::time_t())
                , microsec(0)
            {
            }
        };

        struct log_data {
            int lvl;
            std::string name;
            std::string data;
            log_time when;
            std::thread::id thread;
            std::string thread_data;
            void_call call;
            log_data()
            {
                // std::cerr << "ctor\n";
            }

            ~log_data()
            {
                // std::cerr << "dtor\n";
            }
        };

        using log_data_sptr = std::shared_ptr<log_data>;
        using log_data_uptr = std::unique_ptr<log_data>;

        template <typename T> using cache_trait = cache::traits::unique<T>;
        using cache_type = cache::simple<log_data, std::mutex, cache_trait>;

        //        using cache_type = cache::none<log_data>;
        //        using cache_type = cache::simple<log_data>;

        using queue_element = typename cache_type::value_type;

        ETOOL_OBSERVER_DEFINE(on_write, void(const log_data&));

    public:
        using qresult = etool::queues::condition::result;
        using queue_type = etool::queues::condition::base<queue_element>;

        simple(int lvl)
            : etool::logger::interface(lvl)
            , cache_(10)
        {
        }

        ~simple() {}

        simple(const simple&) = delete;
        simple& operator=(const simple&) = delete;

        static log_time now()
        {
            using std::chrono::duration_cast;
            using microsec = std::chrono::microseconds;
            auto n = std::chrono::high_resolution_clock::now();
            std::time_t tt = std::chrono::high_resolution_clock::to_time_t(n);
            std::uint64_t mm
                = duration_cast<microsec>(n.time_since_epoch()).count();
            return log_time(tt, mm % 1000000);
        }

        void send_data(int lev, const std::string& name,
                       std::string data) override
        {
            using std::chrono::duration_cast;
            using std::chrono::high_resolution_clock;

            // auto element = std::make_shared<log_data>( ); //cache_.get( );
            auto element = cache_.get();

            element->when = now();
            element->lvl = lev;
            element->thread = std::this_thread::get_id();
            element->thread_data = thread_data();
            element->call = void_call();

            element->name.assign(name);
            element->data.swap(data);

            queue_.push(std::move(element));
        }

        void stop()
        {
            queue_.cancel();
        }

        size_t size() const
        {
            return queue_.size();
        }

        bool empty() const
        {
            return queue_.empty();
        }

        void attach()
        {
            queue_element data;
            while (qresult::QUEUE_WAIT_OK == queue_.wait(data)) {
                if (data->call) {
                    data->call();
                } else {
                    on_write(*data);
                }
                cache_.push(std::move(data));
            }
        }

        bool run_one()
        {
            queue_element data;
            bool pop = queue_.pop(data);
            if (pop) {
                if (data->call) {
                    data->call();
                } else {
                    on_write(*data);
                }
                cache_.push(std::move(data));
            }
            return pop;
        }

        void dispatch(void_call call)
        {
            auto element = cache_.get();
            element->call.swap(call);
            queue_.push(std::move(element));
        }

        template <int Id> const std::string& thread_data() const
        {
            return get_thread_data<Id>();
        }

        const std::string& thread_data() const
        {
            return get_thread_data<0>();
        }

        template <int Id> void set_thread_data(std::string data)
        {
            get_thread_data<Id>().swap(std::move(data));
        }

        void set_thread_data(std::string data)
        {
            get_thread_data<0>().swap(data);
        }

    private:
        template <int Id = 0> static std::string& get_thread_data()
        {
            static thread_local std::string data;
            return data;
        }

        queue_type queue_;
        cache_type cache_;
    };

}}

#endif // SIMPLE_H
