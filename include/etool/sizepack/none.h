#ifndef ETOOL_SIZEPACK_NONE_H
#define ETOOL_SIZEPACK_NONE_H

#include <algorithm>
#include <string>

#include <cstdint>

namespace etool { namespace sizepack {

    struct none {

        typedef size_t size_type;

        static const size_t max_length = 0;
        static const size_t min_length = 0;

        static bool valid_length(size_t)
        {
            return true;
        }

        template <typename IterT>
        static size_t packed_length(IterT, const IterT&)
        {
            return 0;
        }

        template <typename IterT>
        static bool valid_packed(const IterT&, const IterT&)
        {
            return true;
        }

        static size_t result_length(size_type)
        {
            return max_length;
        }

        static std::string pack(size_type)
        {
            return std::string();
        }

        static void pack(size_type, std::string&) {}

        static void append(size_type, std::string&) {}

        static size_t pack(size_type, void*)
        {
            return max_length;
        }

        template <typename IterT>
        static size_type unpack(const IterT& begin, const IterT& end)
        {
            return std::distance(begin, end);
        }

        static size_t unpack(const void*, size_t len, size_type* res)
        {
            if (res) {
                *res = len;
            }
            return 0;
        }
    };

}}

#endif // RS_SIZEPACK_NONE_H
