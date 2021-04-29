#ifndef ERROR_H
#define ERROR_H
#include <QRegExp>
#include <QString>
#include "type.h"

class ErrorHandler{
    //use regular expression to fliter invalid instruction
    static QString var;
    static QString exp;
    static QString address;
    static QString stri;
    static QString cmp_op;

public:
    static void throwMsg(Error error);
    static bool isValidExp(StatementType type,char* str);
    static Error StmtToError(StatementType type);

};



#endif // ERROR_H
