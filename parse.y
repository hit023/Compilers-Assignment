%{
    #include<stdio.h>
    int yylex(void);
    int yyerror(const char *s);
    int success = 1;
%}
%start Program
%token IFTOK ELSETOK ID NUM WHILETOK DOTOK BREAKTOK TYPE VOIDTOK RETURNTOK NULLTOK
%nonassoc ELSETOK
%left '+' '-'
%left '*' '/'
%%
Program : Block
| Functions
;
Functions : Functions Function
|
;
Block : '{' Decls Stmts '}'
;
Decls : Decls Decl
|
;
Decl : Type ID ';'
;
Type : Type '[' NUM ']'
| TYPE
;
Stmts : Stmts Stmt
|
;
Stmt : Loc '=' Bool ';'
| IFTOK '(' Bool ')' Stmt
| IFTOK '(' Bool ')' Stmt ELSETOK Stmt
| WHILETOK '(' Bool ')' Stmt
| DOTOK Stmt WHILETOK '(' Bool ')' ';'
| BREAKTOK ';'
| Function_call
| Block
;
Function : TYPE ID '(' Param_list ')' Block
| VOIDTOK ID '(' Param_list ')' Block
;
Param_list : Param_list ',' TYPE ID
| TYPE ID
|
;
Function_call : ID '(' Arglist ')' ';'
;
Arglist : Arglist ',' ID
| ID
|
;
Loc : Loc '[' Bool ']'
| ID
;
Bool : Bool '|' '|' Join
| Join
;
Join : Join '&' '&' Equality
| Equality '!' '=' Rel
| Rel
;
Equality : Equality '=' '=' Rel
| Equality '!' '=' Rel
| Rel
;
Rel : Expr '<' Expr
| Expr '<' '=' Expr
| Expr
;
Expr : Expr '+' Term
| Expr '-' Term
| Term
;
Term : Term '*' Unary
| Term '/' Unary
| Unary
;
Unary : '!' Unary
| '-' Unary
| Factor
;
Factor : '(' Bool ')'
| Loc
| NUM
;
%%
int main (void)
{
    yyparse();
    if(success)
    	printf("Parsing Successful\n");
    return 0;
}

int yyerror(const char *msg)
{
	extern int yylineno;
	printf("Parsing Failed\nLine Number: %d %s\n",yylineno,msg);
	success = 0;
	return 0;
}
