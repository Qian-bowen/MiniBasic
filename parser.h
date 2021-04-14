#ifndef PARSER_H
#define PARSER_H

#include<string>
#include<iostream>
#include<stack>
#include<string.h>
#include<unordered_map>

#include<QDebug>
#include "handle.h"
#include<math.h>

enum ExpressionType { OPERATOR,CONSTANT, IDENTIFIER, COMPOUND ,VIRTUAL};

struct node{
    ExpressionType type;
    char op;//n means no op
    int value;
    std::string content;
    node* left,*right;

    node():op('n'),left(NULL),right(NULL){}
    node(ExpressionType t,char o ,node* l,node* r):
        type(t),op(o),left(l),right(r){}
};

class Parser{
private:
    node* root;
    std::stack<node*> optor,operands;
    std::unordered_map<std::string,int>* mem;

    void calculate_two_op();

    void eval_tree(node* root,int& val,bool& succ);
    void parse_line(std::string exp_str);

    std::string space_create(int level);
    std::string middle_travel(node* root,int level);
    std::string print_node(node* cur,int level);

public:
    Parser(std::unordered_map<std::string,int>* m):mem(m){root=new node;}
    void parse_line_wrapper(std::string exp_str);//handle exception

    bool eval(int& val);
    std::string get_syntax_tree();

};


#endif // PARSER_H
