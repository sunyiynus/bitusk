#ifndef BITUSK_RE_HPP
#define BITUSK_RE_HPP
#include <cstddef>
#include "string.hpp"

/*
 *
 * Mini regex-module inspired by Rob Pike's regex code described in:
 *
 * http://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
 *
 *
 *
 * Supports:
 * ---------
 *   '.'        Dot, matches any character
 *   '^'        Start anchor, matches beginning of string
 *   '$'        End anchor, matches end of string
 *   '*'        Asterisk, match zero or more (greedy)
 *   '+'        Plus, match one or more (greedy)
 *   '?'        Question, match zero or one (non-greedy)
 *   '[abc]'    Character class, match if one of {'a', 'b', 'c'}
 *   '[^abc]'   Inverted class, match if NOT one of {'a', 'b', 'c'} -- NOTE: feature is currently broken!
 *   '[a-zA-Z]' Character ranges, the character set of the ranges { a-z | A-Z }
 *   '\s'       Whitespace, \t \f \r \n \v and spaces
 *   '\S'       Non-whitespace
 *   '\w'       Alphanumeric, [a-zA-Z0-9_]
 *   '\W'       Non-alphanumeric
 *   '\d'       Digits, [0-9]
 *   '\D'       Non-digits
 *
 *
 */

namespace bitusk {



class RegExpr {
public:
    RegExpr() =default;
    bool validate();
};

class Re {
public:
    Re() = default;
    Re(const RegExpr& regexp): m_regexpr(regexp) {}
    void match(const RegExpr& regexp, const bitusk::String& text);
    void match(const bitusk::String& text);
private:


    RegExpr m_regexpr;
};



} // namespace bitusk


#endif // BITUSK_RE_HPP