#ifndef ETOOL_DETAILS_BYTE_SWAP_H
#define ETOOL_DETAILS_BYTE_SWAP_H

#include <cstdint>
#include <string>

namespace etool { namespace details {

    template <typename T>
    struct byte_swap;

    template <typename T>
    struct byte_swap {

        typedef T value_type;

        static void place_swap(value_type& v)
        {
            std::uint8_t* in = reinterpret_cast<std::uint8_t*>(&v);
            static const std::size_t length = sizeof(value_type);
            static const std::size_t middle = length / 2;

            for (std::size_t i = 0; i < middle; ++i) {
                std::uint8_t tmp = in[length - 1 - i];
                in[length - 1 - i] = in[i];
                in[i] = tmp;
            }
        }

        static value_type swap(value_type v)
        {
            place_swap(v);
            return v;
        }
    };

    template <>
    struct byte_swap<std::uint8_t> {

        typedef std::uint8_t value_type;

        static void place_swap(value_type&) {}

        static value_type swap(value_type v)
        {
            return v;
        }
    };

    template <>
    struct byte_swap<std::uint16_t> {
        typedef std::uint16_t value_type;

        static void place_swap(value_type& v)
        {
            v = swap(v);
        }

        static value_type swap(value_type v)
        {
            return ((v & 0xFF00) >> 8) | ((v & 0x00FF) << 8);
        }
    };

    template <>
    struct byte_swap<std::uint32_t> {
        typedef std::uint32_t value_type;

        static void place_swap(value_type& v)
        {
            v = swap(v);
        }

        static value_type swap(value_type v)
        {
            return ((v & 0xFF000000) >> 24) | ((v & 0x00FF0000) >> 8)
                | ((v & 0x0000FF00) << 8) | ((v & 0x000000FF) << 24);
        }
    };

    template <>
    struct byte_swap<std::uint64_t> {
        typedef std::uint64_t value_type;

        static void place_swap(value_type& v)
        {
            v = swap(v);
        }

        static value_type swap(value_type v)
        {
            return ((v & 0xFF00000000000000) >> 56)
                | ((v & 0x00FF000000000000) >> 40)
                | ((v & 0x0000FF0000000000) >> 24)
                | ((v & 0x000000FF00000000) >> 8)
                | ((v & 0x00000000FF000000) << 8)
                | ((v & 0x0000000000FF0000) << 24)
                | ((v & 0x000000000000FF00) << 40)
                | ((v & 0x00000000000000FF) << 56);
        }
    };

    template <>
    struct byte_swap<std::string> {
        typedef std::string value_type;

        static void place_swap(value_type& v)
        {
            auto size = v.size();
            auto middle = size / 2;
            for (std::size_t i = 0; i < middle; ++i) {
                std::swap(v[i], v[size - i - 1]);
            }
        }

        static value_type swap(value_type v)
        {
            place_swap(v);
            return v;
        }
    };

}}

#endif // BYTE_SWAP_H
