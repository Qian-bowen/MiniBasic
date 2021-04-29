#ifndef STATEMENT_H
#define STATEMENT_H
#include "parser.h"
#include "handle.h"
#include "type.h"
#include "memory.h"

class Statement{
protected:
    StatementType type;
    Memory* mem;

    bool stmt_cc;
    int stmt_pc;

    Statement(std::string stmt,Memory* m,StatementType t):type(t),mem(m)
    {stmt_cc=false;stmt_pc=-1;};

public:
    StatementType get_type();
    void get_status(bool& cc,int& pc);
    virtual std::string get_stmt_tree()=0;
    virtual void get_var_name(char*& v){}//inputstmt only
    virtual CompVal get_stmt_eval(){return 0;}
    virtual ~Statement(){}
};


class LetStmt:public Statement{
public:
    Parser* exp;
    char* left_var;

public:
    LetStmt(std::string stmt,Memory* m,StatementType t);
    CompVal get_stmt_eval();
    std::string get_stmt_tree();
    ~LetStmt();

};


class PrintStmt:public Statement{
    Parser* exp;
    CompVal print_val;
public:
    PrintStmt(std::string stmt,Memory* m,StatementType t);
    CompVal get_stmt_eval();//get print value
    std::string get_stmt_tree();
    ~PrintStmt();
};

class PrintfStmt:public Statement{
    Parser* exp;
    CompVal print_val;
public:
    PrintfStmt(std::string stmt,Memory* m,StatementType t);
    CompVal get_stmt_eval();//get printf value
    std::string get_stmt_tree();
    ~PrintfStmt();
};

class RemStmt:public Statement{
public:
    RemStmt(std::string stmt,Memory* m,StatementType t);
    std::string get_stmt_tree(){return "";};
    ~RemStmt();
};

class IfStmt:public Statement{
    Parser* exp1,*exp2;
    char* str1,*str2;
    char op;
public:
    IfStmt(std::string stmt,Memory* m,StatementType t);
    ~IfStmt();
    CompVal get_stmt_eval();//reevaluate exp and cc
    std::string get_stmt_tree();
};

class GotoStmt:public Statement{
public:
    GotoStmt(std::string stmt,Memory* m,StatementType t);
    ~GotoStmt();
    std::string get_stmt_tree();
};

class EndStmt:public Statement{
public:
    EndStmt(std::string stmt,Memory* m,StatementType t);
    ~EndStmt();
    std::string get_stmt_tree(){return "";};
};

class InputStmt:public Statement{
    char* var_name;
public:
    InputStmt(std::string stmt,Memory* m,StatementType t);
    ~InputStmt();
    void get_var_name(char*& v);
    std::string get_stmt_tree(){return "";};
};

class InputsStmt:public Statement{
    char* var_name;
public:
    InputsStmt(std::string stmt,Memory* m,StatementType t);
    ~InputsStmt();
    void get_var_name(char*& v);
    std::string get_stmt_tree(){return "";};
};

#endif // STATEMENT_H
