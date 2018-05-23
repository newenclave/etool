#ifndef ETOOL_SIZEPACK_BLOCKCHAIN_VARINT_H
#define ETOOL_SIZEPACK_BLOCKCHAIN_VARINT_H

#include <algorithm>

#include "etool/details/byte_order.h"

namespace etool { namespace sizepack {

    struct blockchain_varint {

        typedef std::uint64_t size_type;

        static const size_t min_length = sizeof(std::uint8_t);
        static const size_t max_length = sizeof(size_type) + min_length;

        using u16_little = details::byte_order_little<std::uint16_t>;
        using u32_little = details::byte_order_little<std::uint32_t>;
        using u64_little = details::byte_order_little<std::uint64_t>;

        enum prefix_value {

            PREFIX_VARINT_MIN = 0xFD,

            PREFIX_VARINT16 = 0xFD,
            PREFIX_VARINT32 = 0xFE,
            PREFIX_VARINT64 = 0xFF,

            PREFIX_VARINT_MAX = 0xFF,
        };

        static size_t len_by_prefix(std::uint8_t prefix)
        {
            switch (prefix) {
            case PREFIX_VARINT64:
                return sizeof(std::uint64_t) + min_length;
            case PREFIX_VARINT32:
                return sizeof(std::uint32_t) + min_length;
            case PREFIX_VARINT16:
                return sizeof(std::uint16_t) + min_length;
            default:
                break;
            }
            return min_length;
        }

        static bool valid_length(size_t len)
        {
            return (len >= min_length) && (len <= max_length);
        }

        template <typename IterT>
        static size_t packed_length(IterT begin, const IterT& end)
        {
            size_t dist = std::distance(begin, end);
            size_t len = len_by_prefix(*begin);
            return (dist >= len) ? len : 0;
        }

        template <typename IterT>
        static bool valid_packed(const IterT& begin, const IterT& end)
        {
            return valid_length(packed_length(begin, end));
        }

        static size_t result_length(size_type len)
        {
            if (len < PREFIX_VARINT_MIN) {
                return sizeof(std::uint8_t);
            } else if (len <= 0xFFFF) {
                return sizeof(std::uint16_t) + min_length;
            } else if (len <= 0xFFFFFFFF) {
                return sizeof(std::uint32_t) + min_length;
            } else {
                return sizeof(std::uint64_t) + min_length;
            }
        }

        static size_t pack(size_type size, void* result)
        {
            std::uint8_t* res = reinterpret_cast<std::uint8_t*>(result);
            if (size < PREFIX_VARINT_MIN) {
                res[0] = static_cast<std::uint8_t>(size);
                return min_length;
            } else if (size <= 0xFFFF) {
                res[0] = static_cast<std::uint8_t>(PREFIX_VARINT16);
                return u16_little::write(size, &res[1]) + min_length;
            } else if (size <= 0xFFFFFFFF) {
                res[0] = static_cast<std::uint8_t>(PREFIX_VARINT32);
                return u32_little::write(size, &res[1]) + min_length;
            } else {
                res[0] = static_cast<std::uint8_t>(PREFIX_VARINT64);
                return u64_little::write(size, &res[1]) + min_length;
            }
        }

        static void pack(size_type size, std::string& res)
        {
            std::string tmp(result_length(size), '\0');
            pack(size, &tmp[0]);
            res.swap(tmp);
        }

        static std::string pack(size_type size)
        {
            std::string res;
            pack(size, res);
            return res;
        }

        static void append(size_type size, std::string& res)
        {
            size_t last = res.size();
            res.resize(last + result_length(size));
            pack(size, &res[last]);
        }

        template <typename IterT>
        static size_type unpack(IterT begin, const IterT& end)
        {
            size_type res = 0;
            size_t shift = 0;
            size_t len = len_by_prefix(*begin);
            if (len == 1) {
                return static_cast<size_type>(*begin);
            } else {
                for (++begin; (begin != end) && (len > 0); ++begin, --len) {
                    res |= (static_cast<size_type>(
                                static_cast<std::uint8_t>(*begin))
                            << shift);
                    shift += 8;
                }
            }
            return res;
        }

        static size_t unpack(const void* data, size_t length, size_type* res)
        {
            const std::uint8_t* d = static_cast<const std::uint8_t*>(data);
            size_t len = len_by_prefix(*d);

            if (length < len) {
                return 0;
            }

            size_type res_ = 0;

            if (len == min_length) {
                res_ = static_cast<size_type>(*d);
            } else {
                switch (*d) {
                case PREFIX_VARINT16:
                    res_ = u16_little::read(d + min_length);
                    break;
                case PREFIX_VARINT32:
                    res_ = u32_little::read(d + min_length);
                    break;
                case PREFIX_VARINT64:
                    res_ = u64_little::read(d + min_length);
                    break;
                default:
                    break;
                }
            }

            if (res) {
                *res = res_;
            }

            return len;
        }
    };

}}

#endif // BLOCKCHAIN_VARINT_H
