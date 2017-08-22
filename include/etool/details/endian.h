#ifndef ETOOL_DETAILS_ENDIAN_H
#define ETOOL_DETAILS_ENDIAN_H

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

#elif defined( __sparc  ) || defined( __sparc__   ) \
   || defined( _POWER   ) || defined( __powerpc__ ) \
   || defined( __ppc__  ) || defined( __hpux      ) \
   || defined( __s390__ ) || defined( __hppa      ) \
   || defined( _MIPSEB  ) || defined( _POWER      )

#   define ETOOL_BIG_ENDIAN
#   define ETOOL_BYTE_ORDER 4321

#elif defined( __i386__  ) || defined( __alpha__  ) \
   || defined( __ia64    ) || defined( __ia64__   ) \
   || defined( _M_IX86   ) || defined( _M_IA64    ) \
   || defined( _M_ALPHA  ) || defined( __amd64    ) \
   || defined( __amd64__ ) || defined( _M_AMD64   ) \
   || defined( __x86_64  ) || defined( __x86_64__ ) \
   || defined( _M_X64    ) || defined( __bfin__   )

#   define ETOOL_LITTLE_ENDIAN
#   define ETOOL_BYTE_ORDER 1234

#else
#   error Unable to detect bytes order
#endif

#endif // ENDIAN_H
