#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//Declare types and variables used ->
enum DFAstate { START, INNUM, INID, INASSIGN, DONE, INRELOP};

enum tag { MULOP, ADDOP, DIVOP, MINUS, RELOP, IFTOK, ELSETOK, ID, NUM, WHILETOK, ORTOK, ANDTOK, SEMICOL, BRACEOPEN, BRACECLOSE, PAROPEN, PARCLOSE, SQOPEN, SQCLOSE, DOTOK, BREAKTOK, TRUE, FALSE, TYPE, VOIDTOK, RETURNTOK};

typedef enum DFAstate DFAstate;
typedef enum tag tag;

char* keywords[] = {"if", "else", "while", "do", "break", "true", "false", "int", "float", "void", "return"};

tag keyword_tag[] = {IFTOK, ELSETOK, WHILETOK, DOTOK, BREAKTOK, TRUE, FALSE, TYPE, TYPE, VOIDTOK, RETURNTOK};

DFAstate state = START;

typedef struct token{
    tag token_tag;
    char* token_lexeme;
    int token_line_number;
} token;

token* tokens[1001];
int current_token_idx = 0;

//Function prototypes ->
void getTokens(char* program);
int isDigit(char);
int isChar(char);
int isSpace(char);
int isNewline(char);
int isSymbol(char);
int isKeyword(char*);
tag getKeywordTag(int);
void appendToken(token*);
token* newToken(tag,char*,int);
void printTokens();

void getTokens(char* program)
{
    int prog_len = strlen(program),i = 0, line_no = 1;
    char* number;
    char* id;
    int idx = 0,flag = 0;
    while(state != DONE)
    {
        char c = program[i];
        switch(state)
        {
            case START:
                if(isChar(c))
                {
                    state = INID;
                }
                else if(isDigit(c))
                {
                    state = INNUM;
                }
                else if(isSpace(c))
                {
                    ++i;
    				if(i == prog_len) state = DONE;
    				else state = START;
                }
                else if(isSymbol(c))
                {
                    switch(c)
                    {
                        case ';':
                            appendToken(newToken(SEMICOL,";",line_no));
                            ++i;
                            break;
                        case '<':
                            state = INRELOP;
                            break;

                        case '>':
                            state = INRELOP;
                            break;

                        case '=':
                            state = INRELOP;
                            break;

                        case '-':
                            appendToken(newToken(MINUS,"-",line_no));
                            ++i;
                            break;

                        case '+':
                            appendToken(newToken(ADDOP,"-",line_no));
                            ++i;
                            break;

                        case '*':
                            appendToken(newToken(MULOP,"-",line_no));
                            ++i;
                            break;

                        case '/':
                            appendToken(newToken(DIVOP,"-",line_no));
                            ++i;
                            break;

                        case '{':
                            appendToken(newToken(BRACEOPEN,"{",line_no));
                            ++i;
                            break;

                        case '}':
                            appendToken(newToken(BRACECLOSE,"{",line_no));
                            ++i;
                            break;

                        case '[':
                            appendToken(newToken(SQOPEN,"[",line_no));
                            ++i;
                            break;

                        case ']':
                            appendToken(newToken(SQCLOSE,"]",line_no));
                            ++i;
                            break;

                        case '(':
                            appendToken(newToken(PAROPEN,"(",line_no));
                            ++i;
                            break;

                        case ')':
                            appendToken(newToken(PARCLOSE,")",line_no));
                            ++i;
                            break;
                    }
                }
                else if(isNewline(c))
                {
                    ++line_no;
                    ++i;
                }
                else state = DONE;
                break;

            case INNUM:
                //memset(number,0,sizeof(char) * 11);
                number = (char*)malloc(sizeof(char)*11);
                idx = 0, flag= 0;
                while(i<prog_len && isDigit(c))
                {
                    number[idx++] = c;
                    ++i;
                    c = program[i];
                    if(c == '.' && flag == 0)
                    {
                        number[idx++] = c;
                        ++i;
                        c = program[i];
                        flag = 1;
                    }
                }
                number[idx] = '\0';
                appendToken(newToken(NUM,number,line_no));
                if(i == prog_len) state = DONE;
                else state = START;
                break;

            case INID:
                //memset(id,0,100 * sizeof(char));
                id = (char*)malloc(sizeof(char)*100);
                idx= 0;
                while( i<prog_len && (isChar(c) || isDigit(c)) )
                {
                    id[idx++] = c;
                    ++i;
                    c = program[i];
                }
                id[idx] = '\0';
                int is_tok = isKeyword(id);
                if(is_tok != -1)
                {
                    appendToken(newToken(getKeywordTag(is_tok),id,line_no));
                }
                else
                {
                    appendToken(newToken(ID,id,line_no));
                }
                if(i == prog_len) state = DONE;
                else state = START;
                break;

            case INRELOP:
                switch(c)
                {
                    case '<':
                        if(i+1 < prog_len && program[i+1] == '=')
                        {
                            appendToken(newToken(RELOP,"<=",line_no));
                             i += 2;
                        }
                        else if(program[i+1] == '>')
                        {
                            appendToken(newToken(RELOP,"<>",line_no));
                            i += 2;
                        }
                        else
                        {
                            appendToken(newToken(RELOP,"<",line_no));
                            ++i;
                        }
                        state = START;
                        break;

                    case '=':
                        if(i+1 < prog_len && program[i+1] == '=')
                        {
                            appendToken(newToken(RELOP,"==",line_no));
                            i += 2;
                        }
                        else
                        {
                            appendToken(newToken(RELOP,"=",line_no));
                            ++i;
                        }
                        state = START;
                        break;

                    case '>':
                        if(i+1 < prog_len && program[i+1] == '=')
                        {
                            appendToken(newToken(RELOP,">=",line_no));
                            i += 2;
                        }
                        else
                        {
                            appendToken(newToken(RELOP,">",line_no));
                        }
                        state = START;
                        break;
                }
                break;

            case DONE:
                break;
        }
    }
}

void appendToken(token* tok)
{
    tokens[current_token_idx++] = tok;
}

token* newToken(tag tg,char* lexeme,int line)
{
    token* tok = NULL;
    tok = (token*)malloc(sizeof(token));
    tok->token_tag = tg;
    tok->token_lexeme = lexeme;
    tok->token_line_number = line;
    return tok;
}

int isDigit(char c)
{
    return c >= 48 && c <= 57;
}

int isChar(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isSpace(char c)
{
    return (c==' ');
}

int isNewline(char c)
{
    return c=='\n';
}

int isSymbol(char c)
{
    return (c=='<' || c=='>' || c=='=' || c==';' || c=='+' || c=='-' || c=='*' || c=='/' || c=='(' || c==')' || c=='[' || c==']' || c=='{' || c=='}');
}

tag getKeywordTag(int idx)
{
    return keyword_tag[idx];
}

int isKeyword(char* str)
{
    for(int i=0;i<10;i++)
    {
        if(strcmp(str,keywords[i])==0) return i;
    }
    return -1;
}

void printTokens()
{
    for(int i=0;i<current_token_idx;i++)
    {
        printf("Token: %d, string: %s, line: %d\n",tokens[i]->token_tag, tokens[i]->token_lexeme, tokens[i]->token_line_number);
    }
}
