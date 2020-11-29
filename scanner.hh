#pragma once

// I didn't get why we need to use guard
#if ! defined(yyFlexLexerOnce)
// Our lexer is lolFlexLexer
#define yyFlexLexer lolFlexLexer
#include <FlexLexer.h>
#endif

// Say flex what function to implement
#undef YY_DECL
#define YY_DECL lol::parser::symbol_type lol::scanner::scanToken()

#include "parser.hh"

namespace lol {
class scanner: public lolFlexLexer {
public:
    virtual lol::parser::symbol_type scanToken();
};
} // namespace lol