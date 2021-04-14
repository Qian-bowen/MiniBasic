#ifndef LINE_H
#define LINE_H
#include <QString>
#include "type.h"

struct Line{
    int num;
    StatementType type;
    std::string head,content;
    Line(){}
    Line(int n,std::string con,std::string h,StatementType t){num=n;content=con;type=t;head=h;}
    bool operator==(const int& n){
        return (this->num==n);
    }
};

#endif // LINE_H
