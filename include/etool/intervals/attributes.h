#ifndef ETOOL_INTERVALS_ATTRIBUTES_H
#define ETOOL_INTERVALS_ATTRIBUTES_H

#include <cstdint>

namespace etool { namespace intervals {

    enum class attributes: std::uint16_t {
        MIN_INF    = 1 << 0,
        CLOSE      = 1 << 1,
        OPEN       = 1 << 2,
        MAX_INF    = 1 << 3,
    };

    inline constexpr
    attributes operator | ( attributes lh, attributes rh )
    {
        using u16 = std::uint16_t;
        return static_cast<attributes>( static_cast<u16>( lh )
                                      | static_cast<u16>( rh ) );
    }

    inline constexpr
    attributes operator & ( attributes lh, attributes rh )
    {
        using u16 = std::uint16_t;
        return static_cast<attributes>( static_cast<u16>( lh )
                                      & static_cast<u16>( rh ) );
    }

    inline constexpr
    bool operator == ( attributes lh, std::uint16_t val )
    {
        using u16 = std::uint16_t;
        return static_cast<u16>( lh ) == val;
    }

    inline constexpr
    bool operator != ( attributes lh, std::uint16_t val )
    {
        using u16 = std::uint16_t;
        return static_cast<u16>( lh ) != val;
    }

}}

#endif // ATTRIBUTES_H
