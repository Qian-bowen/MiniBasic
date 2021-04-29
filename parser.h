#ifndef PARSER_H
#define PARSER_H

#include<string>
#include<iostream>
#include<stack>
#include<string.h>
#include<unordered_map>

#include<QDebug>
#include "handle.h"
#include "error.h"
#include "memory.h"
#include "expressionNode.h"

class Parser{
private:
    node* root;
    std::stack<node*> optor,operands;
    Memory* mem;

    void calculate_two_op();

    void eval_tree(node* root,CompVal& val,bool& succ);
    void parse_line(std::string exp_str);

    std::string space_create(int level);
    std::string middle_travel(node* root,int level);
    std::string print_node(node* cur,int level);

public:
    Parser(Memory* m):mem(m){root=new node;}
    void parse_line_wrapper(std::string exp_str);//handle exception

    bool eval(CompVal& val);
    std::string get_syntax_tree();

};


#endif // PARSER_H
