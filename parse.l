%option yylineno

%{
    #include "y.tab.h"
    extern int yylval;
%}

%%
"if"            {return IFTOK;}
"else"          {return ELSETOK;}
"break"         {return BREAKTOK;}
"do"            {return DOTOK;}
"void"          {return VOIDTOK;}
"return"        {return RETURNTOK;}
"null"          {return NULLTOK;}
"int"           {return TYPE;}
"float"         {return TYPE;}
"=="			{return DEQ;}
"!="			{return NEQ;}
"<="			{return LE;}
">="			{return GE;}
[ \t]           {return 0;}
[0-9]+(".")?([0-9])*         {return NUM;}
[a-zA-z_][a-zA-z_0-9]* {return ID;}
.               {return yytext[0];}
%%
