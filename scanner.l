%{
    #include <stdlib.h>

    #include "scanner.hh"
    #include "parser.hh"
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

%x INLINE_COMMENT
%x MULTILINE_COMMENT

%%
{blank}+    ;

"BTW"       BEGIN(INLINE_COMMENT);

"OBTW"      BEGIN(MULTILINE_COMMENT);

<INLINE_COMMENT>[^\n]*  ;
<INLINE_COMMENT>\n      BEGIN(INITIAL);

<MULTILINE_COMMENT>"TLDR"  BEGIN(INITIAL);
<MULTILINE_COMMENT>.       ;
<MULTILINE_COMMENT>\n      ;

"HAI"       {
    return lol::parser::make_PROGBEGIN();
}

"KTHXBYE"   {
    return lol::parser::make_PROGEND();
}

"I HAS A"   {
    return lol::parser::make_VAR();
}

"ITZ"       {
    return lol::parser::make_ASSIGN();
}

"R"         {
    return lol::parser::make_REASSIGN();
}

"DIFF OF"   {
    return lol::parser::make_DIFF();
}

"SUM OF"    {
    return lol::parser::make_SUM();
}

"PRODUKT"   {
    return lol::parser::make_PRODUCT();
}

"QUOSHUNT"  {
    return lol::parser::make_SLASH();
}

{int}       {
    return lol::parser::make_int(atoi(yytext));
}

{float}     {
    return lol::parser::make_float(atof(yytext));
}

{id}        {
    return lol::parser::make_id(yytext);
}

{string}    {
    return lol::parser::make_string(yytext);
}

\n          {
    return lol::parser::make_NEWLINE();    
}
%%