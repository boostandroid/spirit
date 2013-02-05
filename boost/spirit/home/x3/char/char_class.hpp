/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_CHAR_CLASS_APRIL_16_2006_1051AM)
#define BOOST_SPIRIT_CHAR_CLASS_APRIL_16_2006_1051AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/char/char_parser.hpp>
#include <boost/spirit/home/x3/char/detail/cast_char.hpp>
#include <boost/spirit/home/support/char_encoding/standard.hpp>
#include <boost/spirit/home/support/char_encoding/standard_wide.hpp>
#include <boost/spirit/home/support/char_encoding/ascii.hpp>
#include <boost/spirit/home/support/char_encoding/iso8859_1.hpp>

namespace boost { namespace spirit { namespace x3
{
    ///////////////////////////////////////////////////////////////////////////
    struct char_tag {};
    struct alnum_tag {};
    struct alpha_tag {};
    struct blank_tag {};
    struct cntrl_tag {};
    struct digit_tag {};
    struct graph_tag {};
    struct print_tag {};
    struct punct_tag {};
    struct space_tag {};
    struct xdigit_tag {};
    struct lower_tag {};
    struct upper_tag {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename Encoding>
    struct char_class_base
    {
        typedef typename Encoding::char_type char_type;

#define BOOST_SPIRIT_CLASSIFY(name)                                             \
        template <typename Char>                                                \
        static bool                                                             \
        is(name##_tag, Char ch)                                                 \
        {                                                                       \
            return Encoding::is##name                                           \
                BOOST_PREVENT_MACRO_SUBSTITUTION                                \
                    (detail::cast_char<char_type>(ch));                         \
        }                                                                       \
        /***/

        BOOST_SPIRIT_CLASSIFY(char)
        BOOST_SPIRIT_CLASSIFY(alnum)
        BOOST_SPIRIT_CLASSIFY(alpha)
        BOOST_SPIRIT_CLASSIFY(digit)
        BOOST_SPIRIT_CLASSIFY(xdigit)
        BOOST_SPIRIT_CLASSIFY(cntrl)
        BOOST_SPIRIT_CLASSIFY(graph)
        BOOST_SPIRIT_CLASSIFY(lower)
        BOOST_SPIRIT_CLASSIFY(print)
        BOOST_SPIRIT_CLASSIFY(punct)
        BOOST_SPIRIT_CLASSIFY(space)
        BOOST_SPIRIT_CLASSIFY(blank)
        BOOST_SPIRIT_CLASSIFY(upper)

#undef BOOST_SPIRIT_CLASSIFY
    };

    template <typename Encoding, typename Tag>
    struct char_class
      : char_parser<char_class<Encoding, Tag>>
    {
        typedef Encoding encoding;
        typedef Tag tag;
        typedef typename Encoding::char_type char_type;
        typedef char_type attribute_type;
        static bool const has_attribute = true;

        template <typename Char, typename Context>
        bool test(Char ch, Context const&) const
        {
            return ((sizeof(Char) <= sizeof(char_type)) || encoding::ischar(ch))
                && char_class_base<Encoding>::is(tag(), ch);
        }
    };

#define BOOST_SPIRIT_CHAR_CLASS(encoding, name)                                 \
    typedef char_class<char_encoding::encoding, name##_tag> name##_type;        \
    name##_type const name = name##_type();                                     \
    /***/

#define BOOST_SPIRIT_CHAR_CLASSES(encoding)                                     \
    namespace encoding                                                          \
    {                                                                           \
        BOOST_SPIRIT_CHAR_CLASS(encoding, alnum)                                \
        BOOST_SPIRIT_CHAR_CLASS(encoding, alpha)                                \
        BOOST_SPIRIT_CHAR_CLASS(encoding, digit)                                \
        BOOST_SPIRIT_CHAR_CLASS(encoding, xdigit)                               \
        BOOST_SPIRIT_CHAR_CLASS(encoding, cntrl)                                \
        BOOST_SPIRIT_CHAR_CLASS(encoding, graph)                                \
        BOOST_SPIRIT_CHAR_CLASS(encoding, lower)                                \
        BOOST_SPIRIT_CHAR_CLASS(encoding, print)                                \
        BOOST_SPIRIT_CHAR_CLASS(encoding, punct)                                \
        BOOST_SPIRIT_CHAR_CLASS(encoding, space)                                \
        BOOST_SPIRIT_CHAR_CLASS(encoding, blank)                                \
        BOOST_SPIRIT_CHAR_CLASS(encoding, upper)                                \
    }                                                                           \
    /***/

    BOOST_SPIRIT_CHAR_CLASSES(standard)
    BOOST_SPIRIT_CHAR_CLASSES(standard_wide)
    BOOST_SPIRIT_CHAR_CLASSES(ascii)
    BOOST_SPIRIT_CHAR_CLASSES(iso8859_1)

#undef BOOST_SPIRIT_CHAR_CLASS
#undef BOOST_SPIRIT_CHAR_CLASSES

    using standard::alnum_type;
    using standard::alpha_type;
    using standard::digit_type;
    using standard::xdigit_type;
    using standard::cntrl_type;
    using standard::graph_type;
    using standard::lower_type;
    using standard::print_type;
    using standard::punct_type;
    using standard::space_type;
    using standard::blank_type;
    using standard::upper_type;

    using standard::alnum;
    using standard::alpha;
    using standard::digit;
    using standard::xdigit;
    using standard::cntrl;
    using standard::graph;
    using standard::lower;
    using standard::print;
    using standard::punct;
    using standard::space;
    using standard::blank;
    using standard::upper;
}}}

#endif
