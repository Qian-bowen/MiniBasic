#ifndef EXPRESSIONNODE_H
#define EXPRESSIONNODE_H

#include <string>
#include "variable.h"

struct node{
    ExpressionType type;
    char op;//n means no op
    CompVal value;
    std::string content;
    node* left,*right;

    node():op('n'),left(NULL),right(NULL){}
    node(ExpressionType t,char o ,node* l,node* r):
        type(t),op(o),left(l),right(r){}
};

#endif // EXPRESSIONNODE_H
