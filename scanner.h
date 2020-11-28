#pragma once

// I didn't get why we need to use guard
#if ! defined(yyFlexLexerOnce)
// Our lexer is lolFlexLexer
#define yyFlexLexer lolFlexLexer
#include <FlexLexer.h>
#endif

// Say flex what function to implement
#undef YY_DECL
#define YY_DECL /*yy::parser::symbol_type*/int lol::LolScanner::scanToken()

namespace lol {
class LolScanner: public lolFlexLexer {
public:
    virtual /*yy::parser::symbol_type*/ int scanToken();
};
} // namespace lol