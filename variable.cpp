#include "variable.h"

CompVal::CompVal(const CompVal& cv)
{
    v_type=cv.v_type;
    v_int=cv.v_int;
    v_str=cv.v_str;
}

CompVal& CompVal::operator=(const CompVal& cv)
{
    v_int=cv.v_int;
    v_str=cv.v_str;
    v_type=cv.v_type;
    return *this;
}

CompVal CompVal::operator+(const CompVal& cv)
{
    return CompVal(v_int+cv.v_int);
}

CompVal CompVal::operator-(const CompVal& cv)
{
    return CompVal(v_int-cv.v_int);
}

CompVal CompVal::operator*(const CompVal& cv)
{
    return CompVal(v_int*cv.v_int);
}

CompVal CompVal::operator/(const CompVal& cv)
{
    return CompVal(v_int/cv.v_int);
}

CompVal CompVal::operator^(const CompVal& cv)
{
    return CompVal(pow(v_int,cv.v_int));
}

bool CompVal::operator==(const CompVal& cv)
{
    return ((v_int==cv.v_int && v_type==V_INT && cv.v_type==V_INT)||
            (v_str==cv.v_str && v_type==V_STR && cv.v_type==V_STR));
}

bool CompVal::operator>(const CompVal& cv)
{
    bool result=false;
    result=(v_type==V_INT&&cv.v_type==V_INT&&v_int>cv.v_int);
    return result;
}

bool CompVal::operator<(const CompVal& cv)
{
    bool result=false;
    result=(v_type==V_INT&&cv.v_type==V_INT&&v_int<cv.v_int);
    return result;
}
