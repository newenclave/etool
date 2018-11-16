#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <mutex>

#include "etool/details/type_uid.h"
#include "etool/subsystems/iface.h"

namespace etool { namespace subsystems {
    template <typename MutexType = std::recursive_mutex>
    class base_root : public iface {

        template <typename T>
        using uid_type = etool::details::type_uid<T>;
        using mutex_type = MutexType;

    public:
        base_root() = default;
        base_root(const base_root&) = delete;
        base_root& operator=(const base_root&) = delete;

        template <typename SubSys>
        auto get() -> typename uid_type<SubSys>::value_type*
        {
            using UidType = uid_type<SubSys>;
            using value_type = typename UidType::value_type;
            static const auto id = UidType::get();
            return static_cast<value_type*>(get_iface(id));
        }

        template <typename SubSys>
        auto get() const -> const typename uid_type<SubSys>::value_type*
        {
            using UidType = uid_type<SubSys>;
            using value_type = typename UidType::value_type;
            static const auto id = UidType::get();
            return static_cast<const value_type*>(get_iface(id));
        }

        template <typename SubSys>
        auto get_ref() -> typename uid_type<SubSys>::value_type&
        {
            auto ptr = get<SubSys>();
            if (!ptr) {
                throw std::out_of_range("Subsystem is missing");
            }
            return *ptr;
        }

        template <typename SubSys>
        auto get_ref() const -> const typename uid_type<SubSys>::value_type&
        {
            auto ptr = get<SubSys>();
            if (!ptr) {
                throw std::out_of_range("Subsystem is missing");
            }
            return *ptr;
        }

        template <typename T, typename... Args>
        void add(Args&&... args)
        {
            using stype = typename uid_type<T>::value_type;
            std::unique_ptr<iface> inst(new stype(std::forward<Args>(args)...));
            std::lock_guard<mutex_type> lock(ifaces_lock_);
            ifaces_order_.push_back(inst.get());
            ifaces_.insert(std::make_pair(uid_type<T>::get(), std::move(inst)));
        }

        void init() override
        {
            for_each([](auto s) { s->init(); });
        }

        void start() override
        {
            for_each([](auto s) { s->start(); });
        }

        void stop() override
        {
            for_each([](auto s) { s->stop(); });
        }

        template <typename Call>
        void for_each(Call call)
        {
            std::lock_guard<mutex_type> lock(ifaces_lock_);
            for (auto s : ifaces_order_) {
                call(s);
            }
        }

        std::size_t size() const
        {
            std::lock_guard<mutex_type> lock(ifaces_lock_);
            return ifaces_.size();
        }

    private:
        iface* get_iface(std::uintptr_t id)
        {
            std::lock_guard<mutex_type> lock(ifaces_lock_);
            auto itr = ifaces_.find(id);
            return (itr == ifaces_.end()) ? nullptr : itr->second.get();
        }

        const iface* get_iface(std::uintptr_t id) const
        {
            std::lock_guard<mutex_type> lock(ifaces_lock_);
            auto itr = ifaces_.find(id);
            return (itr == ifaces_.end()) ? nullptr : itr->second.get();
        }

        mutable mutex_type ifaces_lock_;
        std::map<std::uintptr_t, std::unique_ptr<iface>> ifaces_;
        std::list<iface*> ifaces_order_;
    };

    using root = base_root<std::recursive_mutex>;
}}
