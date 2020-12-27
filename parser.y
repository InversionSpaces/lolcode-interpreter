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

    #include "ast.hh"

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
    END     0   "EOF"
    NEWLINE     "\n"
    PROGBEGIN   "HAI"
    PROGEND     "KTHXBYE"
    VAR         "I HAS A"
    ASSIGN      "ITZ"
    REASSIGN    "R"
    SUM         "SUM OF"
    DIFF        "DIFF OF"
    PRODUCT     "PRODUKT"
    SLASH       "QUOSHUNT"
    AND         "AN"
    IF          "O RLY?"
    THEN        "YA RLY"
    ELSEIF      "MEBBE"
    ELSE        "NO WAI"
    ENDIF       "OIC"
;

// Declaration of tokens with values
%token <int>            int     "int"
%token <float>          float   "float"
%token <bool>           bool    "bool"
%token <std::string>    string  "string"
%token <std::string>    id      "id"

// Declaration of nonterminals with values
%nterm <ast::StmtRep>       stmt
%nterm <ast::BlockRep>      block
%nterm <ast::AssignRep>     assign
%nterm <ast::IfStmtRep>     ifstmt
%nterm <ast::ExprRep>       expr
%nterm <ast::ConstantRep>   literal

%%
%start programm;

programm: 
    PROGBEGIN string[version] NLS block[code] PROGEND 
    { driver.result = ast::AST($version, $code); }

block: 
    %empty 
    { $$ = ast::util::create<ast::Block>(); }
    | stmt block[other]
    { $$ = ast::util::create<ast::Block>($stmt, $other); }

stmt: 
    assign
    { $$ = $assign; }
    | ifstmt
    { $$ = $ifstmt; }

assign:
    VAR id NLS
    { $$ = 
        ast::util::create<ast::Assign>(
            $id, ast::util::create<ast::Constant>(ast::untyped)
        ); 
    }
    | VAR id ASSIGN expr[value] NLS
    { $$ = ast::util::create<ast::Assign>($id, $value); }
    | id REASSIGN expr[value] NLS
    { $$ = ast::util::create<ast::Assign>($id, $value); }

ifstmt:
    expr[condition] IF NLS THEN NLS block[then] ENDIF NLS
    { $$ = ast::util::create<ast::IfStmt>($condition, $then); }
    | expr[condition] IF NLS THEN NLS block[then] ELSE NLS block[otherwise] ENDIF NLS
    { $$ = ast::util::create<ast::IfStmt>($condition, $then, $otherwise); }

expr:
    literal[value]
    { $$ = $value; }
    | SUM expr[lhs] AND expr[rhs]
    { $$ = ast::util::create<ast::Sum>($lhs, $rhs); }
    | DIFF expr[lhs] AND expr[rhs]
    { $$ = ast::util::create<ast::Diff>($lhs, $rhs); }
    | PRODUCT expr[lhs] AND expr[rhs]
    { $$ = ast::util::create<ast::Product>($lhs, $rhs); }
    | SLASH expr[lhs] AND expr[rhs]
    { $$ = ast::util::create<ast::Slash>($lhs, $rhs); }

literal:
    int[val]
    { $$ = ast::util::create<ast::Constant>($val); }
    | float[val]
    { $$ = ast::util::create<ast::Constant>($val); }
    | string[val]
    { $$ = ast::util::create<ast::Constant>($val); }
    | bool[val]
    { $$ = ast::util::create<ast::Constant>($val); }

NLS: NEWLINE | NLS NEWLINE
%%

// Error function for parser
void lol::parser::error(const lol::location& loc, const std::string& m)
{
    cerr << loc << " : " << m << endl;
}