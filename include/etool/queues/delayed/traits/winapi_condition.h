#ifndef ETOOL_DELAYED_QUEUE_TRAIT_WINAPI_CONDITIONAL_H
#define ETOOL_DELAYED_QUEUE_TRAIT_WINAPI_CONDITIONAL_H

#if defined(_WIN32) && defined(_WIN32_WINNT_VISTA)

#include <Windows.h>
#include <chrono>

namespace etool { namespace queues { namespace delayed {

    namespace traits {
        struct winapi_condition {
            class cs_mutex {
            public:
                cs_mutex(const cs_mutex &) = delete;
                cs_mutex& operator = (const cs_mutex &) = delete;

                cs_mutex(cs_mutex &&other)
                {
                    swap(other);
                }

                cs_mutex& operator = (cs_mutex && other)
                {
                    swap(other);
                    return *this;
                }

                cs_mutex()
                {
                    InitializeCriticalSection(&cs_);
                }

                ~cs_mutex() 
                {
                    DeleteCriticalSection(&cs_);
                }

                void swap(cs_mutex &other)
                {
                    std::swap(cs_, other.cs_);
                }

                void lock()
                {
                    EnterCriticalSection(&cs_);
                }

                void unlock()
                {
                    LeaveCriticalSection(&cs_);
                }

                bool try_lock()
                {
                    return TryEnterCriticalSection(&cs_) != FALSE;
                }

                PCRITICAL_SECTION get()
                {
                    return &cs_;
                }
            private:
                CRITICAL_SECTION cs_;
            };
            using mutex_type = cs_mutex;

            class cv_wrapper {
            public:
                cv_wrapper()
                {
                    InitializeConditionVariable(&cv_);
                }
                cv_wrapper(cv_wrapper &&other)
                {
                    swap(other);
                }

                cv_wrapper & operator = (cv_wrapper && other)
                {
                    swap(other);
                    return *this;
                }

                cv_wrapper(const cv_wrapper &) = delete;
                cv_wrapper & operator = (const cv_wrapper &) = delete;

                void notify_one()
                {
                    WakeConditionVariable(&cv_);
                }

                void notify_all()
                {
                    WakeAllConditionVariable(&cv_);
                }

                template <typename Pred>
                void wait(std::unique_lock<mutex_type> &lock, Pred pred)
                {
                    while (!pred()) {
                        BOOL res = SleepConditionVariableCS(&cv_, 
                                             lock.mutex()->get(), INFINITE);
                        if (!res) {
                            std::error_code ec(GetLastError(), 
                                               std::system_category());
                            throw std::system_error(std::move(ec));
                        }
                    }
                }

                template <typename TimePoint, typename Pred>
                bool wait_until(std::unique_lock<mutex_type> &lock,
                                TimePoint point, Pred pred)
                {
                    using namespace std::chrono;
                    if (pred()) {
                        return true;
                    }
                    auto now = steady_clock::now();
                    while(point > now) {
                        auto diff = duration_cast<milliseconds>
                                            (point - now).count();
                        BOOL res = SleepConditionVariableCS(&cv_, 
                                            lock.mutex()->get(), 
                                            static_cast<DWORD>(diff));
                        if (!res) {
                            if (GetLastError() == ERROR_TIMEOUT) {
                                return false;
                            } else {
                                std::error_code ec(GetLastError(),
                                    std::system_category());
                                throw std::system_error(std::move(ec));
                            }
                        } else if (pred()) {
                            return true;
                        } else {
                            now = steady_clock::now();
                        }
                    }
                    return false;
                }

                void swap(cv_wrapper &other)
                {
                    std::swap(cv_, other.cv_);
                }
            private:
                CONDITION_VARIABLE cv_;
            };
            using condition_variable_type = cv_wrapper;
        };
    }

} } }

#endif

#endif
