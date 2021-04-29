#ifndef LINE_H
#define LINE_H

#include "error.h"
#include "statement.h"

struct Line{
    int num;
    StatementType type;
    Error error;
    std::string head,content;
    Line(){}
    Line(int n,std::string con,std::string h,StatementType t,Error e){num=n;content=con;type=t;head=h;error=e;}
    bool operator==(const int& n){
        return (this->num==n);
    }
    Error get_line_error(){return error;}
};

#endif // LINE_H
