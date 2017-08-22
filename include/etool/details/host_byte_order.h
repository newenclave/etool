#ifndef ETOOL_HOST_BYTE_ORDER_H
#define ETOOL_HOST_BYTE_ORDER_H

/// copy-paste from Boost
///
#if defined(__GLIBC__) || defined(__gnu_linux__)

#   include <endian.h>
#   if (__BYTE_ORDER == __LITTLE_ENDIAN)
#       define ETOOL_LITTLE_ENDIAN
#   elif (__BYTE_ORDER == __BIG_ENDIAN)
#       define ETOOL_BIG_ENDIAN
#   elif (__BYTE_ORDER == __PDP_ENDIAN)
#       define ETOOL_PDP_ENDIAN
#   else
#       error Unknown machine endianness detected.
#   endif
#   define ETOOL_BYTE_ORDER __BYTE_ORDER

#elif defined(_BIG_ENDIAN)    && !defined(_LITTLE_ENDIAN)      || \
    defined(__BIG_ENDIAN__)   && !defined(__LITTLE_ENDIAN__)   || \
    defined(_STLP_BIG_ENDIAN) && !defined(_STLP_LITTLE_ENDIAN)

# define ETOOL_BIG_ENDIAN
# define ETOOL_BYTE_ORDER 4321

#elif defined(_LITTLE_ENDIAN)    && !defined(_BIG_ENDIAN)      || \
    defined(__LITTLE_ENDIAN__)   && !defined(__BIG_ENDIAN__)   || \
    defined(_STLP_LITTLE_ENDIAN) && !defined(_STLP_BIG_ENDIAN)

#   define ETOOL_LITTLE_ENDIAN
#   define ETOOL_BYTE_ORDER 1234

#elif defined(__sparc)  || defined(__sparc__)                 \
   || defined(_POWER)   || defined(__powerpc__)               \
   || defined(__ppc__)  || defined(__hpux) || defined(__hppa) \
   || defined(_MIPSEB)  || defined(_POWER)                    \
   || defined(__s390__)

#   define ETOOL_BIG_ENDIAN
#   define ETOOL_BYTE_ORDER 4321

#elif defined(__i386__)  || defined(__alpha__)  \
   || defined(__ia64)    || defined(__ia64__)   \
   || defined(_M_IX86)   || defined(_M_IA64)    \
   || defined(_M_ALPHA)  || defined(__amd64)    \
   || defined(__amd64__) || defined(_M_AMD64)   \
   || defined(__x86_64)  || defined(__x86_64__) \
   || defined(_M_X64)    || defined(__bfin__)

#   define ETOOL_LITTLE_ENDIAN
#   define ETOOL_BYTE_ORDER 1234

#else
#   error Unable to detect bytes order
#endif


namespace etool { namespace details {

/* just test. Doesn't work
class endian
{
private:

    static constexpr uint32_t ut32 = 0x01020304;
    static constexpr uint8_t  byte = static_cast<const uint8_t &>(ut32);

public:

    static constexpr bool is_little = ( byte == 0x04 );
    static constexpr bool is_middle = ( byte == 0x02 );
    static constexpr bool is_big    = ( byte == 0x01 );

    static_assert( is_little || is_middle || is_big,
                   "Cannot determine endianness!" );
private:

    endian( ) = delete;

};
*/

    enum class endian {
        BIG     = 0,
        LITTLE  = 1,
    };

    struct host_byte_order {

        static
        endian get( )
        {
            static const endian eness[2] = { endian::LITTLE, endian::BIG };
            typedef const unsigned short const_ushort;
            static const int res =
                 (*reinterpret_cast<const_ushort *>("\001") == 0x0100) ? 1 : 0;
            return eness[res];
        }

        static
        bool is_big_endian( )
        {
            static const bool res = (get( ) == endian::BIG);
            return res;
        }

        static
        bool is_little_endian( )
        {
            static const bool res = (get( ) == endian::LITTLE);
            return res;
        }
    };

}}

#endif // HOST_BYTE_ORDER_H
