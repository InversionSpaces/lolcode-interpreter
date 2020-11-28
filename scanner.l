%{
    #include <iostream>
    #include "scanner.h"

    using namespace std;
%}

%option noyywrap nounput noinput batch debug
%option c++
%option prefix="lol"
%option yyclass="LolScanner"

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+
float [0-9]+\.[0-9]*
blank [ \t\r]

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

{int}       {
    cout << "Found int: " << yytext << endl;
    return 10;
}

{float}     {
    cout << "Found float: " << yytext << endl;
    return 11;
}

{id}        {
    cout << "Found id: " << yytext << endl;
    return 12;
}

\n          {
    cout << "Newline found" << endl;
}
%%

int main() {
    lol::LolScanner scanner;
    while (scanner.scanToken()) {
        cout << "Token scanned" << endl;
    }
}