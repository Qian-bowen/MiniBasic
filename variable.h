#ifndef VARIABLE_H
#define VARIABLE_H
#include<string>
#include<math.h>
#include "type.h"

class CompVal{
    VarType v_type;
    int v_int;
    std::string v_str;

public:
    CompVal(){};
    CompVal(int v){v_type=V_INT;v_int=v;}
    CompVal(std::string s){v_type=V_STR;v_str=s;}
    CompVal(const CompVal& cv);
    VarType get_type(){return v_type;}
    CompVal& operator=(const CompVal& cv);
    CompVal operator+(const CompVal& cv);
    CompVal operator-(const CompVal& cv);
    CompVal operator*(const CompVal& cv);
    CompVal operator/(const CompVal& cv);
    CompVal operator^(const CompVal& cv);
    bool operator>(const CompVal& cv);
    bool operator<(const CompVal& cv);
    bool operator==(const CompVal& cv);
    int get_int_val(){return v_int;};
    std::string get_str_val(){return v_str;};
};


#endif // VARIABLE_H
