#include <bits/stdc++.h>
using namespace std;

enum DFAstate { START, INNUM, INID, INASSIGN, DONE, INRELOP, INIF};

typedef enum DFAstate DFAstate;

string terminals[] = {"IFTOK", "ELSETOK", "WHILETOK", "DOTOK",
 "BREAKTOK", "TYPE", "VOIDTOK", "RETURNTOK", "*", "+",
 "/", "-",";", "{", "}", "(", ")","&", "!"
 "[", "]", "ORTOK", "ANDTOK", "NUM", "ID", ",", "<", ">", "GE", "LE", "DEQ", "NEQ",
 "NULLTOK", "=", "|", "$end"};

string keywords[] = {"if","else", "while", "do","break", "void", "return", "int", "float","NULL"};

int nterminals = 36;
int nkeywords = 10;

DFAstate stat = START;

typedef struct token{
    int token_id;
    string token_lexeme;
    int token_line_number;
} token;

map<int,string> inv;
map<string,int> mp;

vector<token> tokens;
int current_token_idx = 0;
int is_tok = -1;

//Function prototypes ->
void getTokens(string program);
int isDigit(char);
int isChar(char);
int isSpace(char);
int isNewline(char);
int isSymbol(char);
int isKeyword(string);
int getTokenID(string);
void appendToken(token);
token newToken(int,string,int);
void printTokens();
int isTerminal(string s);

string removeComments(const string& s)
{
    string ret = "";
    int len = s.length();
    for(int i=0;i<len;i++)
    {
        if(s[i] == '/' && i+1 < len && s[i+1]=='/')
        {
            //cout<<"entered\n";
            while(s[i]!='\n' && i < len)
            {
                //cout<<"omitting: "<<s[i]<<'\n';
                ++i;
            }
            if(i < len) ret += s[i];
        }
        else ret += s[i];
    }
    string g = "";
    len = ret.length();
    for(int i=0;i<len;i++)
    {
        if(ret[i] == '/' && i+1 < len && ret[i+1] == '*')
        {
            while(i < len && !(i+1 < len && ret[i] == '*' && ret[i+1] == '/')) ++i;
            if(i >= len)
            {
                return "Error in multiline comments";
            }
            ++i;
        }
        else g += ret[i];
    }
    return g;
}

void getTokens(string program)
{
    program = removeComments(program);
    if(program == "Error")
    {
        cout<<"Error in scanning comments.";
        return;
    }
    int prog_len = program.length(),i = 0, line_no = 1;
    string number;
    string id;
    int idx = 0,flag = 0;
    while(stat != DONE)
    {
        char c = program[i];
        switch(stat)
        {
            case START:
                if(isChar(c))
                {
                    stat = INID;
                }
                else if(isDigit(c))
                {
                    stat = INNUM;
                }
                else if(isSpace(c))
                {
                    ++i;
    				if(i == prog_len) stat = DONE;
    				else stat = START;
                }
                else if(isSymbol(c))
                {
                    switch(c)
                    {
                        case ';':
                            appendToken(newToken(mp[";"],";",line_no));
                            ++i;
                            break;

                        case '<':
                            stat = INRELOP;
                            break;

                        case '>':
                            stat = INRELOP;
                            break;

                        case '=':
                            stat = INRELOP;
                            break;

                        case '!':
                            stat = INRELOP;
                            break;

                        case '-':
                            appendToken(newToken(mp["-"],"-",line_no));
                            ++i;
                            break;

                        case '+':
                            appendToken(newToken(mp["+"],"-",line_no));
                            ++i;
                            break;

                        case '*':
                            appendToken(newToken(mp["*"],"-",line_no));
                            ++i;
                            break;

                        case '/':
                            appendToken(newToken(mp["/"],"-",line_no));
                            ++i;
                            break;

                        case '{':
                            appendToken(newToken(mp["{"],"{",line_no));
                            ++i;
                            break;

                        case '}':
                            appendToken(newToken(mp["}"],"}",line_no));
                            ++i;
                            break;

                        case '[':
                            appendToken(newToken(mp["["],"[",line_no));
                            ++i;
                            break;

                        case ']':
                            appendToken(newToken(mp["]"],"]",line_no));
                            ++i;
                            break;

                        case '(':
                            appendToken(newToken(mp["("],"(",line_no));
                            ++i;
                            break;

                        case ')':
                            appendToken(newToken(mp[")"],")",line_no));
                            ++i;
                            break;

                        case ',':
                            appendToken(newToken(mp[","], ",", line_no));
                            ++i;
                            break;

                        default:
                            cout << "Error in line: "<<line_no<<'\n';
                            ++i;
                    }
                }
                else if(isNewline(c))
                {
                    ++line_no;
                    ++i;
                }
                else stat = DONE;
                break;

            case INNUM:
                //memset(number,0,sizeof(char) * 11);
                //number = (string)malloc(sizeof(char)*11);
                number.clear();
                flag= 0;
                while(i<prog_len && isDigit(c))
                {
                    //number[idx++] = c;
                    number += c;
                    ++i;
                    c = program[i];
                    if(c == '.' && flag == 0)
                    {
                        //number[idx++] = c;
                        number += c;
                        ++i;
                        c = program[i];
                        flag = 1;
                    }
                }
                //number[idx] = '\0';
                //cout<<"bye\n";
                //cout<<number<<'\n';
                appendToken(newToken(mp["NUM"],number,line_no));
                if(i == prog_len) stat = DONE;
                else stat = START;
                break;

            case INID:
                id.clear();
                while( i<prog_len && (isChar(c) || isDigit(c)) )
                {
                    id += c;
                    ++i;
                    c = program[i];
                }
                is_tok = isKeyword(id);
                if(is_tok != -1)
                {
                    appendToken(newToken(getTokenID(id),id,line_no));
                }
                else
                {
                    appendToken(newToken(mp["ID"],id,line_no));
                }
                if(i == prog_len) stat = DONE;
                else stat = START;
                break;

            case INRELOP:
                switch(c)
                {
                    case '!':
                        if(i+1 < prog_len && program[i+1] == '=')
                        {

                            appendToken(newToken(mp["NEQ"],"!=",line_no));
                            i += 2;
                        }
                        break;

                    case '<':
                        if(i+1 < prog_len && program[i+1] == '=')
                        {
                            appendToken(newToken(mp["LE"],"<=",line_no));
                             i += 2;
                        }
                        else
                        {
                            appendToken(newToken(mp["<"],"<",line_no));
                            ++i;
                        }
                        stat = START;
                        break;

                    case '=':
                        if(i+1 < prog_len && program[i+1] == '=')
                        {
                            appendToken(newToken(mp["DEQ"],"==",line_no));
                            i += 2;
                        }
                        else
                        {
                            appendToken(newToken(mp["="],"=",line_no));
                            ++i;
                        }
                        stat = START;
                        break;

                    case '>':
                        if(i+1 < prog_len && program[i+1] == '=')
                        {
                            appendToken(newToken(mp["GE"],">=",line_no));
                            i += 2;
                        }
                        else
                        {
                            appendToken(newToken(mp[">"],">",line_no));
                        }
                        stat = START;
                        break;
                }
                break;

            case DONE:
                break;
        }
    }
    appendToken(newToken(mp["$end"],"$end",line_no));
}

void appendToken(token tok)
{
    //tokens[current_token_idx++] = tok;
    tokens.push_back(tok);
    current_token_idx++;
}

token newToken(int id,string lexeme,int line)
{
    token tok{};
    tok.token_id = id;
    tok.token_lexeme = lexeme;
    tok.token_line_number = line;
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
    return (c=='<' || c=='>' || c=='=' || c==';' || c=='+' || c=='-' || c=='*' || c=='/' || c=='(' || c==')' || c=='[' || c==']' || c=='{' || c=='}' || c==',' || c == '?');
}

int isKeyword(string str)
{
    for(int i=0;i<nkeywords;i++)
    {
        if(str == keywords[i]) return i;
    }
    return -1;
}

int getTokenID(string s)
{
    if(s == "if") return mp["IFTOK"];
    if(s == "else") return mp["ELSETOK"];
    if(s == "while") return mp["WHILETOK"];
    if(s == "do") return mp["DOTOK"];
    if(s == "break") return mp["BREAKTOK"];
    if(s == "return") return mp["RETURNTOK"];
    if(s == "int" || s == "float") return mp["TYPE"];
    if(s == "void") return mp["VOIDTOK"];
}

void printTokens()
{
    for(int i=0;i<current_token_idx;i++)
    {
        //cout<<"Token: "<<inv[tokens[i].token_id]<<", string: "<<tokens[i].token_lexeme<<", line: "<<tokens[i].token_line_number<<"\n";
        cout<<"Line: "<<tokens[i].token_line_number<<"  "<<tokens[i].token_lexeme<<"           "<<inv[tokens[i].token_id]<<"\n";
    }
}

int isTerminal(string s)
{
    for(int i=0;i<nterminals;i++)
    {
        if(s == terminals[i]) return 1;
    }
    return 0;
}
