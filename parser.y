%skeleton "lalr1.cc"
%require "3.7"

// Write a parser header file containing definitions
// for the token kind names defined in the grammar 
// as well as a few other declarations
%defines

// Enable locations support
%locations

// This option with next makes 
// yylex to have signature 
// as below
%define api.token.constructor

// Declare value type - variant
// Like union, but for C++
// Can store any C++ type
%define api.value.type variant

// API namespace
%define api.namespace {lol}

// Token prefixes
%define api.token.prefix {LOLTOK_}

%code requires {
    #include <string>

    namespace lol {
        class scanner;
        class driver;
    }
//
    #include <iostream>
    using namespace std;
}

// Pass scanner as parameter to yylex
%lex-param {lol::scanner& scanner}

// Pass scanner as parameter to parser
%parse-param {lol::scanner& scanner}
// Pass driver as parameter to parser
%parse-param {lol::driver& driver}

%code {
    #include "scanner.hh"
    #include "driver.hh"

    static lol::parser::symbol_type yylex(
        lol::scanner& scanner
    ) {
        // this will update scanner.location
        return scanner.scanToken();
    }
}

// Declaration of tokens without values
%token
    END 0 "EOF"
    NEWLINE "\n"
    PROGBEGIN "HAI"
    PROGEND "KTHXBYE"
    VAR "I HAS A"
    ASSIGN "ITZ"
    REASSIGN "R"
    DIFF "DIFF OF"
    SUM "SUM OF"
    PRODUCT "PRODUKT"
    SLASH "QUOSHUNT"
;

// Declaration of tokens with values
%token <std::string> id "identifier"
%token <int> int "integer"
%token <float> float "float"
%token <std::string> string "string"

%%
%start programm;

programm: PROGBEGIN string NEWLINE codeblock PROGEND {
    cout << "programm matched" << endl;
};

codeblock: %empty
    | statement codeblock;

statement: varstmt {
    cout << "statement matched" << endl;
};

varstmt: VAR id ASSIGN expr NEWLINE;

expr: int { cout << "int matched" << endl; }
    | float { cout << "float matched" << endl; }
    | string { cout << "string matched" << endl; };
%%

// Error function for parser
void lol::parser::error(const lol::location& loc, const std::string& m)
{
    cerr << loc << " : " << m << endl;
}