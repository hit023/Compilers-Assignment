#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <bits/stdc++.h>
#include "tokens.h"
namespace pt = boost::property_tree;
using namespace std;

typedef struct rule{
    int lhs; //non-terminal
    vector<int> rhs;
}rule;

typedef struct state
{
    int state_id;
    vector<pair<int,int> > items;
    map<int,int> go_to;//for nonterminals
    map<int,int> action;//for terminals
    int red_rule; //reduction state
}state;

typedef struct parse_table
{
    void load(const string &filename);
    void initialise(const string &filename);
}parse_table;

vector<state> states;
vector<rule> rules;
set<int> err_state1;
set<int> err_state2;

void parse_table::initialise(const string& filename)
{
    pt::ptree tree;
    pt::read_xml(filename, tree);

    BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("grammar.nonterminals"))
    {
        string nt = v.second.get<string>("<xmlattr>.name");
        int n = v.second.get<int>("<xmlattr>.symbol-number");
        mp[nt] = n;
        inv[n] = nt;
    }

    BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("grammar.terminals"))
    {
        string g = v.second.get<string>("<xmlattr>.name");
        if(g[0] == '\'')
        {
            g.erase(g.begin());
            g.erase(g.begin() + g.length()-1);
        }
        int n = v.second.get<int>("<xmlattr>.symbol-number");
        mp[g] = n;
        inv[n] = g;
    }
    mp["~"] = 1000;
    inv[1000] = "~";
}

void parse_table::load(const string &filename)
{
    pt::ptree tree;
    pt::read_xml(filename, tree);

    int idx = 0;

    //get rules.
    BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("grammar.rules")) {
        string l = v.second.get<string>("lhs");
        rule rl{};
        rl.lhs = mp[l];
        pt::ptree & subtree = v.second;
        for(pt::ptree::value_type &k: subtree.get_child("rhs"))
        {
            string g = k.second.data();
            if(g[0] == '\'')
            {
                g.erase(g.begin());
                g.erase(g.begin() + g.length()-1);
            }
            rl.rhs.push_back(mp[g]);
        }
        rules.push_back(rl);
    }

    BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("automaton"))
    {
        pt::ptree & subtree = v.second;
        state st{};
        st.state_id  =v.second.get<int>("<xmlattr>.number");
        for(pt::ptree::value_type &k: subtree.get_child("itemset"))
        {
            int f = k.second.get<int>("<xmlattr>.rule-number");
            int s = k.second.get<int>("<xmlattr>.point");
            st.items.push_back({f,s});
        }
        int f=0,x=0;
        for(pt::ptree::value_type &k: subtree.get_child("actions.transitions"))
        {
            string g = k.second.get<string>("<xmlattr>.symbol");
            string type = k.second.get<string>("<xmlattr>.type");
            int s = k.second.get<int>("<xmlattr>.state");
            if(g[0] == '\'')
            {
                g.erase(g.begin());
                g.erase(g.begin() + g.length()-1);
            }
            if(type == "goto")
            {
                st.go_to[mp[g]] = s;
            }
            else
            {
                //++cnt;
                st.action[mp[g]] = s;
                if(g == ";")
                {
                    f = 1;
                }
                else if(g == "}")
                {
                    x = 1;
                }
            }
        }
        if(f==1)
        {
            err_state1.insert(st.state_id);
            //cout << "Error state: "<<st.state_id<<'\n';
        }
        if(x == 1)
        {
            err_state2.insert(st.state_id);
        }
        for(pt::ptree::value_type &k: subtree.get_child("actions.reductions"))
        {
            string s = k.second.get<string>("<xmlattr>.rule");
            if(s == "accept")
            {
                st.red_rule = -1;
            }
            else
            {
                st.red_rule = stoi(s); //convert string to int.
            }
        }
        states.push_back(st);
    }
}

/*int main()
{
    try
    {
        parse_table pt;
        pt.initialise("ml");
        pt.load("ml");
        for(auto i : states)
        {
            cout<<"state: "<<i.state_id<<'\n';
            for(auto j : i.items)
            {
                cout<<j.first<<' '<<j.second<<'\n';
            }
        }
        cout << "Success\n";
    }
    catch (exception &e)
    {
        cout << "Error: " << e.what() << "\n";
    }
    return 0;
}*/
