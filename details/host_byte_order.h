#ifndef ETOOL_HOST_BYTE_ORDER_H
#define ETOOL_HOST_BYTE_ORDER_H


namespace etool { namespace details {

    struct host_byte_order {

        static bool is_big_endian( )
        {
            typedef const unsigned short const_ushort;
            static const bool res =
                    (*reinterpret_cast<const_ushort *>("\001") == 0x0100);
            return res;
        }

        static bool is_little_endian( )
        {
            return !is_big_endian( );
        }
    };

}}

#endif // HOST_BYTE_ORDER_H
