#ifndef TYPE_H
#define TYPE_H
/*
 * minibasic mode
*/
enum Mode{DEBUG,OTHER};
/*
 * Statement type
*/
enum StatementType{LET,IF,PRINT,GOTO,REM,INPUT,INPUTS,END,PRINTF,INVALID};

/*
 * expression type
*/
enum ExpressionType { OPERATOR,CONSTANT, IDENTIFIER, COMPOUND,STRING ,VIRTUAL};

/*
 * variable type
*/
enum VarType{V_INT,V_STR,V_DBL};

/*
 * handle error
 * E_PRO_MISS: program missing
 * E_TYPE_INCOMP: the two operands cannot implement calculation
*/
enum Error{E_NONE,E_PRO_MISS,E_LINE_MISS,E_INS_LET,E_INS_PRINT,E_INS_PRINTF,
           E_INS_GOTO,E_INS_END,E_INS_IF,E_INS_INPUT,
           E_INS_NOT,E_ZERO,E_UDEF_VAR,E_EXP,E_STRING_OP,
           E_IVD_ADDR,E_TYPE_INCOMP};

#endif // TYPE_H
