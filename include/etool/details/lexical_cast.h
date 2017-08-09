#ifndef ETOOL_DETAILS_LEXICAL_CAST_H
#define ETOOL_DETAILS_LEXICAL_CAST_H

namespace etool { namespace details {

    template <typename FromT, typename ToT>
    struct lexical_cast_impl;

//    template <>
//    struct lexical_cast_impl<double>;


    template <typename FromT, typename ToT>
    ToT lexical_cast( FromT from )
    {

    }

    template <typename FromT>
    FromT lexical_cast( FromT from )
    {
        return from;
    }

}}

#endif // LEXICAL_CAST_H
