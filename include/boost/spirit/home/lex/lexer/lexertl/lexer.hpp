//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_LEX_LEXER_MAR_17_2007_0139PM)
#define BOOST_SPIRIT_LEX_LEXER_MAR_17_2007_0139PM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <iosfwd>

#include <boost/spirit/home/support/safe_bool.hpp>
#include <boost/spirit/home/support/detail/lexer/generator.hpp>
#include <boost/spirit/home/support/detail/lexer/rules.hpp>
#include <boost/spirit/home/support/detail/lexer/consts.hpp>
#include <boost/spirit/home/support/unused.hpp>

#include <boost/spirit/home/lex/lexer/lexertl/token.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/functor.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/iterator.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/unique_id.hpp>
#if defined(BOOST_SPIRIT_LEXERTL_DEBUG)
#include <boost/spirit/home/support/detail/lexer/debug.hpp>
#endif

namespace boost { namespace spirit { namespace lex { namespace lexertl
{ 
    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////
        //  The must_escape function checks if the given character value needs
        //  to be preceded by a backslash character to disable its special 
        //  meaning in the context of a regular expression
        ///////////////////////////////////////////////////////////////////////
        template <typename Char>
        inline bool must_escape(Char c)
        {
            // FIXME: more needed?
            switch (c) {
            case '+': case '/': case '*': case '?':
            case '|':
            case '(': case ')':
            case '[': case ']':
            case '{': case '}':
            case '.':
            case '^': case '$':
            case '\\':
            case '"':
                return true;

            default:
                break;
            }
            return false;
        }

        ///////////////////////////////////////////////////////////////////////
        //  The escape function returns the string representation of the given 
        //  character value, possibly escaped with a backslash character, to 
        //  allow it being safely used in a regular expression definition.
        ///////////////////////////////////////////////////////////////////////
        template <typename Char>
        inline std::basic_string<Char> escape(Char ch) 
        { 
            std::basic_string<Char> result(1, ch);
            if (detail::must_escape(ch)) 
            {
                typedef typename std::basic_string<Char>::size_type size_type;
                result.insert((size_type)0, 1, '\\');
            }
            return result;
        }

        ///////////////////////////////////////////////////////////////////////
        //  
        ///////////////////////////////////////////////////////////////////////
        inline boost::lexer::regex_flags map_flags(unsigned int flags)
        {
            unsigned int retval = boost::lexer::none;
            if (flags & match_flags::match_not_dot_newline)
                retval |= boost::lexer::dot_not_newline;
            if (flags & match_flags::match_icase)
                retval |= boost::lexer::icase;

            return boost::lexer::regex_flags(retval);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    //  token_set
    ///////////////////////////////////////////////////////////////////////////
    template <typename Token, typename Iterator = typename Token::iterator_type>
    class token_set
    {
    protected:
        typedef typename boost::detail::iterator_traits<Iterator>::value_type 
            char_type;
        typedef std::basic_string<char_type> string_type;

    public:
        typedef Token token_type;
        typedef typename Token::id_type id_type;

        token_set(unsigned int flags = 0)
          : rules_(detail::map_flags(flags), &unique_id<id_type>::get)
        {}

        // interface for token definition management
        std::size_t add_token (char_type const* state, char_type tokendef
          , std::size_t token_id)
        {
            return rules_.add(state, detail::escape(tokendef), token_id, state);
        }

        std::size_t add_token (char_type const* state, string_type const& tokendef
          , std::size_t token_id)
        {
            return rules_.add(state, tokendef, token_id, state);
        }

        // interface for pattern definition management
        std::size_t add_pattern (char_type const* state, string_type const& name
          , string_type const& patterndef)
        {
            add_state(state);
            return rules_.add_macro(name.c_str(), patterndef);
        }

        boost::lexer::rules const& get_rules() const { return rules_; }

        void clear() { rules_.clear(); }

        std::size_t add_state(char_type const* state)
        {
            return rules_.add_state(state);
        }
        string_type initial_state() const 
        { 
            return string_type(rules_.initial());
        }

    private:
        boost::lexer::basic_rules<char_type> rules_;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Lexer>
    bool generate_static(Lexer const& lex, std::ostream& os, char const* name);

    ///////////////////////////////////////////////////////////////////////////
    //
    //  Every lexer type to be used as a lexer for Spirit has to conform to 
    //  the following public interface:
    //
    //    typedefs: 
    //        iterator_type   The type of the iterator exposed by this lexer.
    //        token_type      The type of the tokens returned from the exposed 
    //                        iterators.
    //        token_set       The type of the token set representing a lexer 
    //                        state.
    //
    //    functions:
    //        default constructor
    //                        Since lexers are instantiated as base classes 
    //                        only it might be a good idea to make this 
    //                        constructor protected.
    //        begin, end      Return a pair of iterators, when dereferenced
    //                        returning the sequence of tokens recognized in 
    //                        the input stream given as the parameters to the 
    //                        begin() function.
    //        add_token       Should add the definition of a token to be 
    //                        recognized by this lexer.
    //        clear           Should delete all current token definitions
    //                        associated with the given state of this lexer 
    //                        object.
    //
    //    template parameters:
    //        Iterator        The type of the iterator used to access the
    //                        underlying character stream.
    //        Token           The type of the tokens to be returned from the
    //                        exposed token iterator.
    //        Functor         The type of the InputPolicy to use to instantiate
    //                        the multi_pass iterator type to be used as the 
    //                        token iterator (returned from begin()/end()).
    //        TokenSet        The type of the token set to use in conjunction 
    //                        with this lexer type. This is used for the 
    //                        token_set typedef described above only.
    //
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    //
    //  The lexer class is a implementation of a Spirit.Lex lexer on 
    //  top of Ben Hanson's lexertl library as outlined above (For more 
    //  information about lexertl go here: http://www.benhanson.net/lexertl.html).
    //
    //  This class is supposed to be used as the first and only template 
    //  parameter while instantiating instances of a lex::lexer class.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename Token = token<>
      , typename Iterator = typename Token::iterator_type
      , typename Functor = functor<Token, Iterator, mpl::false_>
      , typename TokenSet = lex::token_set<token_set<Token, Iterator> > >
    class lexer 
    {
    public:
        // operator_bool() is needed for the safe_bool base class
        operator typename safe_bool<lexer>::result_type() const 
        { 
            return safe_bool<lexer>()(initialized_dfa_); 
        }

        typedef typename boost::detail::iterator_traits<Iterator>::value_type 
            char_type;
        typedef std::basic_string<char_type> string_type;

        //  Every lexer type to be used as a lexer for Spirit has to conform to 
        //  a public interface .
        typedef Token token_type;
        typedef typename Token::id_type id_type;
        typedef TokenSet token_set;
        typedef iterator<Functor> iterator_type;

    private:
        // this type is purely used for the iterator_type construction below
        struct iterator_data_type 
        {
            boost::lexer::basic_state_machine<char_type> const& state_machine_;
            boost::lexer::basic_rules<char_type> const& rules_;
            typename Functor::semantic_actions_type const& actions_;
        };

    public:
        //  Return the start iterator usable for iterating over the generated
        //  tokens.
        iterator_type begin(Iterator& first, Iterator const& last) const
        { 
            if (!init_dfa())
                return iterator_type();

            iterator_data_type iterator_data = { state_machine_, rules_, actions_ };
            return iterator_type(iterator_data, first, last);
        }

        //  Return the end iterator usable to stop iterating over the generated 
        //  tokens.
        iterator_type end() const
        { 
            return iterator_type(); 
        }

    protected:
        //  Lexer instances can be created by means of a derived class only.
        lexer(unsigned int flags) 
          : flags_(detail::map_flags(flags))
          , rules_(flags_, &unique_id<id_type>::get)
          , initialized_dfa_(false)
        {}

    public:
        // interface for token definition management
        std::size_t add_token(char_type const* state, char_type tokendef, 
            std::size_t token_id)
        {
            add_state(state);
            initialized_dfa_ = false;
            return rules_.add(state, detail::escape(tokendef), token_id, state);
        }
        std::size_t add_token(char_type const* state, string_type const& tokendef, 
            std::size_t token_id)
        {
            add_state(state);
            initialized_dfa_ = false;
            return rules_.add(state, tokendef, token_id, state);
        }

        // Allow a token_set to be associated with this lexer instance. This 
        // copies all token definitions of the right hand side into this lexer
        // instance.
        void add_token(char_type const* state, token_set const& tokset)
        {
            add_state(state);
            initialized_dfa_ = false;
            rules_.add("*", tokset.get_rules(), state);
        }

        // Allow to associate a whole lexer instance with another lexer 
        // instance. This copies all token definitions of the right hand side 
        // lexer into this instance.
        template <typename Token_, typename Iterator_, typename Functor_
          , typename TokenSet_>
        void add_token(char_type const* state
          , lexer<Token_, Iterator_, Functor_, TokenSet_> const& lexer_def)
        {
            add_state(state);
            initialized_dfa_ = false;
            rules_.add("*", lexer_def.get_rules(), state);
        }

        // interface for pattern definition management
        void add_pattern (char_type const* state, string_type const& name,
            string_type const& patterndef)
        {
            add_state(state);
            rules_.add_macro(name.c_str(), patterndef);
            initialized_dfa_ = false;
        }

        boost::lexer::rules const& get_rules() const { return rules_; }

        void clear(char_type const* state)
        {
            std::size_t s = rules_.state(state);
            if (boost::lexer::npos != s)
                rules_.clear(state);
            initialized_dfa_ = false;
        }
        std::size_t add_state(char_type const* state)
        {
            std::size_t stateid = rules_.state(state);
            if (boost::lexer::npos == stateid) {
                stateid = rules_.add_state(state);
                initialized_dfa_ = false;
            }
            return stateid;
        }
        string_type initial_state() const 
        { 
            return string_type(rules_.initial());
        }

        //  Register a semantic action with the given id
        template <typename F>
        void add_action(id_type unique_id, std::size_t state, F act)
        {
            // If you get compilation errors below stating value_type not being
            // a member of boost::fusion::unused_type, then you are probably
            // using semantic actions in your token definition without 
            // the actor_lexer being specified as the base class of your lexer
            // (instead of the lexer class).
            typedef typename Functor::semantic_actions_type::value_type
                value_type;
            typedef typename Functor::wrap_action_type wrapper_type;

            if (actions_.size() <= state)
                actions_.resize(state + 1); 

            value_type& actions (actions_[state]);
            if (actions.size() <= unique_id)
                actions.resize(unique_id + 1); 

            actions[unique_id] = wrapper_type::call(act);
        }

        bool init_dfa() const
        {
            if (!initialized_dfa_) {
                state_machine_.clear();
                typedef boost::lexer::basic_generator<char_type> generator;
                generator::build (rules_, state_machine_);
                generator::minimise (state_machine_);

#if defined(BOOST_SPIRIT_LEXERTL_DEBUG)
                boost::lexer::debug::dump(state_machine_, std::cerr);
#endif
                initialized_dfa_ = true;
            }
            return true;
        }

    private:
        // lexertl specific data
        mutable boost::lexer::basic_state_machine<char_type> state_machine_;
        std::size_t unique_ids_;
        boost::lexer::regex_flags flags_;
        boost::lexer::basic_rules<char_type> rules_;

        typename Functor::semantic_actions_type actions_;
        mutable bool initialized_dfa_;

        template <typename Lexer> 
        friend bool generate_static(Lexer const&, std::ostream&, char const*);
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  The actor_lexer class is another implementation of a Spirit.Lex 
    //  lexer on top of Ben Hanson's lexertl library as outlined above (For 
    //  more information about lexertl go here: 
    //  http://www.benhanson.net/lexertl.html).
    //
    //  The only difference to the lexer class above is that token_def
    //  definitions may have semantic (lexer) actions attached while being 
    //  defined:
    //
    //      int w;
    //      token_def word = "[^ \t\n]+";
    //      self = word[++ref(w)];        // see example: word_count_lexer
    //
    //  This class is supposed to be used as the first and only template 
    //  parameter while instantiating instances of a lex::lexer class.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename Token = token<>
      , typename Iterator = typename Token::iterator_type
      , typename Functor = functor<Token, Iterator, mpl::true_>
      , typename TokenSet = lex::token_set<token_set<Token, Iterator> > >
    class actor_lexer : public lexer<Token, Iterator, Functor, TokenSet>
    {
    protected:
        //  Lexer instances can be created by means of a derived class only.
        actor_lexer(unsigned int flags) 
          : lexer<Token, Iterator, Functor, TokenSet>(flags) {}
    };

}}}}

#endif