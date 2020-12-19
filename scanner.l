%{
    #include <stdlib.h>

    // generated
    #include "lolparser.hh"

    #include "scanner.hh"
%}

%option noyywrap nounput noinput batch debug
%option c++
%option prefix="lol"
%option yyclass="scanner"

id      [a-zA-Z][a-zA-Z_0-9]*
int     [0-9]+
float   [0-9]+\.[0-9]*
string  \"[^\"]*\"
blank   [ \t\r]

/* Additional states */
%x INLINE_COMMENT
%x MULTILINE_COMMENT

%{
    // Code run each time a pattern is matched.
    #define YY_USER_ACTION loc.columns(yyleng);
%}

%%
%{
    // Code run each time yylex is called.
    // To move location
    loc.step();
%}

{blank}+    loc.step();

"BTW"       BEGIN(INLINE_COMMENT);

"OBTW"      BEGIN(MULTILINE_COMMENT);

<INLINE_COMMENT>[^\n]*  ;
<INLINE_COMMENT>\n      BEGIN(INITIAL);

<MULTILINE_COMMENT>"TLDR"  BEGIN(INITIAL);
<MULTILINE_COMMENT>.       ;
<MULTILINE_COMMENT>\n      ;

"HAI"       return lol::parser::make_PROGBEGIN  (loc);
"KTHXBYE"   return lol::parser::make_PROGEND    (loc);
"I HAS A"   return lol::parser::make_VAR        (loc);
"ITZ"       return lol::parser::make_ASSIGN     (loc);
"R"         return lol::parser::make_REASSIGN   (loc);
"DIFF OF"   return lol::parser::make_DIFF       (loc);
"SUM OF"    return lol::parser::make_SUM        (loc);
"PRODUKT"   return lol::parser::make_PRODUCT    (loc);
"QUOSHUNT"  return lol::parser::make_SLASH      (loc);
"WIN"       return lol::parser::make_TRUE       (loc);
"FAIL"      return lol::parser::make_FALSE      (loc);

{int}       return lol::parser::make_int        (atoi(yytext), loc);
{float}     return lol::parser::make_float      (atof(yytext), loc);
{id}        return lol::parser::make_id         (yytext, loc);
{string}    return lol::parser::make_string     (yytext, loc);

\n          {
    // Keep track of location
    // yyleng is 1
    loc.lines(yyleng); loc.step();
    return lol::parser::make_NEWLINE(loc);
}

<<EOF>>     return lol::parser::make_END        (loc);
%%