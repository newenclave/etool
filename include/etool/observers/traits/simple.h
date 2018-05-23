#ifndef ETOOL_OBSERVERS_SLOTS_SIMPLE_H
#define ETOOL_OBSERVERS_SLOTS_SIMPLE_H

#include <functional>

namespace etool { namespace observers { namespace traits {

    template <typename SigType> struct simple {

        typedef std::function<SigType> value_type;

        static void erase(value_type&) {}

        static bool expired(value_type&)
        {
            return false;
        }

        template <typename... Args>
        static void exec(value_type& self, Args&&... args)
        {
            self(std::forward<Args>(args)...);
        }
    };

}}}

#endif // SIMPLE_H
