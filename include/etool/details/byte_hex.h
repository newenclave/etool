#ifndef ETOOL_DETAILS_BYTE_HEX_H
#define ETOOL_DETAILS_BYTE_HEX_H

#include <cstdint>
#include "etool/details/result.h"

namespace etool { namespace details {

    struct byte_hex {
        using result_type = detail::result<std::string, std::string>;

    };

}}

#endif // BYTE_HEX_H
