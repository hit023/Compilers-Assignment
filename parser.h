#include <bits/stdc++.h>
#include "make_table.h"
using namespace std;

void print_stack_token(stack<token> st)
{
	while(!st.empty())
	{
		cout<<st.top().token_lexeme<<' ';
		st.pop();
	}
}

void print_stack_state(stack<state> st)
{
	while(!st.empty())
	{
		cout<<st.top().state_id<<' ';
		st.pop();
	}
}

void parse(vector<token> &tokens)
{
	stack<state> st_state;
	stack<token> st_token;
	st_state.push(states[0]);
	int len = tokens.size();
	int i = 0;
	token prev;
	while(1)
	{
		state tp = st_state.top();
		token tk;
		if(i >= tokens.size()) tk = prev;
		else tk = tokens[i];
		prev = tk;
		string lex = tk.token_lexeme;
		cout<<"state stack: \n";
		print_stack_state(st_state);
		cout<<'\n';
		cout<<"input stack: \n";
		print_stack_token(st_token);
		cout<<'\n';
		cout<<"Input token: "<<inv[tk.token_id]<<'\n';
		cout<<'\n';
		if(isTerminal(inv[tk.token_id]))
		{
			string h = inv[tk.token_id];
			if(tp.action.find(mp[h]) != tp.action.end())
			{
				//shift
				st_token.push(tk);
				st_state.push(states[tp.action[mp[h]]]);
				++i;
			}
			else if(err_state1.find(tp.state_id) != err_state1.end())
			{
				cout << "Was Expecting ';' around line : "<<tk.token_line_number<<"\n";
				st_token.push(newToken(mp[";"],";",tk.token_line_number));
				st_state.push(states[tp.action[mp[";"]]]);
			}
			else if(err_state2.find(tp.state_id) != err_state2.end())
			{
				cout << "Was Expecting '}' around line : "<<tk.token_line_number<<"\n";
				st_token.push(newToken(mp["}"],"}",tk.token_line_number));
				st_state.push(states[tp.action[mp["}"]]]);
			}
			else
			{
				//reduce; implies only one item is there
				int red_rule = tp.red_rule;
				if(red_rule == -1)
				{
					cout<<"Accepted!\n";
					break;
				}
				rule rl = rules[red_rule];
				int rlen = rl.rhs.size();
				if(rlen==1 && rl.rhs[0] == 1000)
				{
					st_token.push(newToken(rl.lhs,inv[rl.lhs],tk.token_line_number));
					st_state.push(states[tp.go_to[rl.lhs]]);
				}
				else
				{
					cout<<"Reducing...\n";
					for (int j = 0; j < rlen; ++j)
					{
						if(!st_token.empty()) st_token.pop();
						else
						{
							cout<<"Error in parsing in line : " << tk.token_line_number <<'\n';
							return;
						}
						if(!st_state.empty()) st_state.pop();
						else
						{
							cout<<"Error in parsing in line : " << tk.token_line_number <<'\n';
							return;
						}
					}
					st_token.push(newToken(rl.lhs,inv[rl.lhs],tk.token_line_number));
					st_state.push(states[st_state.top().go_to[rl.lhs]]);
				}
			}
		}
		else
		{
			cout<<"Got a non-term ---- Quitting.\n";
			return;
		}
	}
}

/*int main()
{
	parse_table pt;
    pt.initialise("ml");
	pt.load("ml");
	vector<token> v;
	v.push_back(newToken(mp["TYPE"],"int",1));
	v.push_back(newToken(mp["ID"],"main",1));
	v.push_back(newToken(mp["("],"(",1));
	v.push_back(newToken(mp[")"],")",1));
	v.push_back(newToken(mp["{"],"{",2));
	v.push_back(newToken(mp["TYPE"],"int",2));
	v.push_back(newToken(mp["ID"],"a",2));
	v.push_back(newToken(mp[";"],";",2));
	v.push_back(newToken(mp["RETURNTOK"],"return",3));
	v.push_back(newToken(mp["NUM"],"0",3));
	v.push_back(newToken(mp[";"],";",3));
	v.push_back(newToken(mp["}"],"}",4));
	v.push_back(newToken(mp["$end"],"$end",-1));
	parse(v);
	for(auto i : rules)
	{
		cout<<inv[i.lhs]<<" : ";
		for(auto j : i.rhs)
		{
			cout<<inv[j]<<' ';
		}
		cout<<'\n';
	}
	return 0;
}*/
