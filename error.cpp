#include "error.h"

/*
 * FILTER INVALID CMD/EXP FORMAT
 * DETAILED HANDLER WILL BE IMPLEMENTED WHEN PARSING
 * var cannot begin with digit
 * string cannot contain quotation inside the outer quotation
*/
QString ErrorHandler::var="[a-zA-Z][0-9a-zA-Z]*";
QString ErrorHandler::exp="[0-9a-zA-Z\\s+\\-*/()]+";
QString ErrorHandler::address="[0-9]+";
QString ErrorHandler::cmp_op="[><=]";
QString ErrorHandler::stri="[\"\'][^\'\"]*[\"\']";

/*
 * throw error message
*/
void ErrorHandler::throwMsg(Error error)
{
    switch(error)
    {
    case E_LINE_MISS:
        throw("LINE NUMBER MISSING");
        break;
    case E_INS_LET:
        throw("INVALID INSTRUCTION LET");
        break;
    case E_INS_PRINT:
        throw("INVALID INSTRUCTION PRINT");
        break;
    case E_INS_GOTO:
        throw("INVALID INSTRUCTION GOTO");
        break;
    case E_INS_END:
        throw("INVALID INSTRUCTION END");
        break;
    case E_INS_INPUT:
        throw("INVALID INSTRUCTION INPUT");
        break;
    case E_INS_IF:
        throw("INVALID INSTRUCTION IF");
        break;
    case E_INS_NOT:
        throw("INSTRUCTION NOT EXIST");
        break;
    case E_ZERO:
        throw("DIVIDE BY ZERO");
        break;
    case E_UDEF_VAR:
        throw("UNDEFINED VAR");
        break;
    case E_EXP:
        throw("INVALID EXPRESSION");
        break;
    case E_STRING_OP:
        throw("STRING CANNOT BE OPRAND");
        break;
    case E_IVD_ADDR:
        throw("INVALID GOTO ADDRESS");
        break;
    case E_TYPE_INCOMP:
        throw("INVALID OPERAND TYPE");
        break;
    default:
        break;
    }
    return;
}

/*
 * handle regular expression
*/
bool ErrorHandler::isValidExp(StatementType type,char* str)
{
    bool valid=false;

    switch(type)
    {
    case LET:
    {
        QRegExp let("^\\s*"+var+"\\s*=\\s*"+exp+"\\s*$");
        QRegExp let_str("^\\s*"+var+"\\s*=\\s*"+stri+"\\s*$");
        valid=(let.exactMatch(QString(str))||let_str.exactMatch(QString(str)));
        break;
    }
    case PRINT:
    {
        QRegExp print("^\\s*"+exp+"\\s*$");
        valid=print.exactMatch(QString(str));
        break;
    }
    case GOTO:
    {
        QRegExp go_to("^\\s*"+address+"\\s*$");
        valid=go_to.exactMatch(QString(str));
        break;
    }
    case END:
    {
        QRegExp end("^\\s*$");
        valid=end.exactMatch(QString(str));
        break;
    }
    case IF:
    {
        QRegExp ifreg("^\\s*"+exp+"\\s*"+cmp_op+"\\s*"+exp
                      +"\\s*"+"THEN"+"\\s*"+address+"\\s*$");
        valid=ifreg.exactMatch(QString(str));
        break;
    }
    case INPUT:
    case INPUTS:
    {
        QRegExp input("^\\s*"+var+"\\s*$");
        valid=input.exactMatch(QString(str));
        break;
    }
    default:
        break;
    }
    return valid;
}

/*
 * convert from statement type to error type
*/
Error ErrorHandler::StmtToError(StatementType stmt)
{
    Error error=E_NONE;
    switch(stmt)
    {
    case LET:
    {
        error=E_INS_LET;
        break;
    }
    case IF:
    {
        error=E_INS_IF;
        break;
    }
    case PRINT:
    {
        error=E_INS_PRINT;
        break;
    }
    case GOTO:
    {
        error=E_INS_GOTO;
        break;
    }
    case INPUT:
    {
        error=E_INS_INPUT;
        break;
    }
    case END:
    {
        error=E_INS_END;
        break;
    }
    default:
        break;
    }
    return error;
}
