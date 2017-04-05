#ifndef ETOOL_HOST_BYTE_ORDER_H
#define ETOOL_HOST_BYTE_ORDER_H


namespace etool { namespace details {

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
