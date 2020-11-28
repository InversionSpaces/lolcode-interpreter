%skeleton "lalr1.cc"
%require "3.7"

// Write a parser header file containing definitions
// for the token kind names defined in the grammar 
// as well as a few other declarations
%defines

// Declare value type - variant
// Like union, but for C++
// Can store any C++ type
%define api.value.type variant

%code requires {
    #include <string>
}

// Declaration of tokens without values
%token
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
%token <std::string> IDENTIFIER "identifier"
%token <int> INTEGER "integer"
%token <float> FLOAT "float"
%token <std::string> STRING "string"

%%
%start programm
programm: PROGBEGIN STRING codeblock PROGEND;

codeblock: %empty
    | statement codeblock;

statement: varstmt;

varstmt: VAR IDENTIFIER ASSIGN expr;

expr: NUMBER;
%%