#pragma once

#include <fstream>
#include <string>

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
#include "location.hh"

namespace lol {
class scanner: public lolFlexLexer {
    lol::location loc;
public:
    void restart(
        const std::string& fname,
        std::ifstream& file    
    );

    virtual lol::parser::symbol_type scanToken();
};
} // namespace lol