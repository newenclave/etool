#ifndef RS_SIZEPACK_TYPES_H
#define RS_SIZEPACK_TYPES_H

#include <cstdint>

namespace etool { namespace sizepack {

    template <int BITS>
    struct types;

    template<>
    struct types<sizeof(std::uint8_t)> {
        typedef std::int8_t         signed_type;
        typedef std::uint8_t        unsigned_type;
        static const unsigned_type  top_bit         = 0x80;
        static const unsigned_type  top_bit_shift   = 7;
    };

    template<>
    struct types<sizeof(std::uint16_t)> {
        typedef std::int16_t        signed_type;
        typedef std::uint16_t       unsigned_type;
        static const unsigned_type  top_bit         = 0x8000;
        static const unsigned_type  top_bit_shift   = 15;
    };

    template<>
    struct types<sizeof(std::uint32_t)> {
        typedef std::int32_t        signed_type;
        typedef std::uint32_t       unsigned_type;
        static const unsigned_type  top_bit         = 0x80000000;
        static const unsigned_type  top_bit_shift   = 31;
    };

    template<>
    struct types<sizeof(std::uint64_t)> {
        typedef std::int64_t        signed_type;
        typedef std::uint64_t       unsigned_type;
        static const unsigned_type  top_bit         = 0x8000000000000000;
        static const unsigned_type  top_bit_shift   = 63;
    };

}}

#endif // TYPES_H
