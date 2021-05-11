#ifndef ERROR_H
#define ERROR_H
#include <QRegExp>
#include <QString>
#include <regex>
#include<iostream>//test
#include "type.h"

class ErrorHandler{
    //use regular expression to fliter invalid instruction
    static QString var;
    static QString exp;
    static QString address;
    static QString stri;
    static QString cmp_op;
    static QString printf_format_sect;

public:
    static void throwMsg(Error error);
    static bool isValidExp(StatementType type,char* str);
    static Error StmtToError(StatementType type);

};

class RunError{
    int pc;
    const char* msg;
public:
    RunError(int p,const char* m){pc=p;msg=m;}
    int get_error_pc(){return pc;}
    const char* get_error_msg(){return msg;}
};



#endif // ERROR_H
