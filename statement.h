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
    virtual void get_var_name(char*& v)=0;//inputstmt only
    virtual CompVal get_stmt_eval()=0;
    //virtual std::vector<CompVal> get_stmt_eval_array()=0;
    virtual ~Statement(){}
};


class LetStmt:public Statement{
public:
    Parser* exp;
    char* left_var;

public:
    LetStmt(std::string stmt,Memory* m,StatementType t);
    CompVal get_stmt_eval();
    //std::vector<CompVal> get_stmt_eval_array(){return {};}
    std::string get_stmt_tree();
    void get_var_name(char*& v){};
    ~LetStmt();

};


class PrintStmt:public Statement{
    Parser* exp;
    CompVal print_val;
public:
    PrintStmt(std::string stmt,Memory* m,StatementType t);
    CompVal get_stmt_eval();//get print value
    //std::vector<CompVal> get_stmt_eval_array(){return {};}
    std::string get_stmt_tree();
    void get_var_name(char*& v){};
    ~PrintStmt();
};

class PrintfStmt:public Statement{
    std::vector<CompVal> val_array;
    std::string origin_stmt;
    std::string row_string;
    int argv_num;
public:
    PrintfStmt(std::string stmt,Memory* m,StatementType t);
    CompVal get_stmt_eval(){return CompVal();}//get printf value
    //std::vector<CompVal> get_stmt_eval_array();
    std::string get_stmt_tree(){return "";};//todo!
    void get_var_name(char*& v);
    ~PrintfStmt();
};

class RemStmt:public Statement{
public:
    RemStmt(std::string stmt,Memory* m,StatementType t);
    CompVal get_stmt_eval(){return CompVal();}
    //std::vector<CompVal> get_stmt_eval_array(){return {};}
    std::string get_stmt_tree(){return "";}
    void get_var_name(char*& v){};
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
    //std::vector<CompVal> get_stmt_eval_array(){return {};}
    std::string get_stmt_tree();
    void get_var_name(char*& v){}
};

class GotoStmt:public Statement{
public:
    GotoStmt(std::string stmt,Memory* m,StatementType t);
    ~GotoStmt();
    CompVal get_stmt_eval(){return CompVal();}
    //std::vector<CompVal> get_stmt_eval_array(){return {};}
    std::string get_stmt_tree();
    void get_var_name(char*& v){}
};

class EndStmt:public Statement{
public:
    EndStmt(std::string stmt,Memory* m,StatementType t);
    ~EndStmt();
    CompVal get_stmt_eval(){return CompVal();}
    //std::vector<CompVal> get_stmt_eval_array(){return {};}
    std::string get_stmt_tree(){return "";}
    void get_var_name(char*& v){}
};

class InputStmt:public Statement{
    char* var_name;
public:
    InputStmt(std::string stmt,Memory* m,StatementType t);
    ~InputStmt();
    void get_var_name(char*& v);
    CompVal get_stmt_eval(){return CompVal();}
    //std::vector<CompVal> get_stmt_eval_array(){return {};}
    std::string get_stmt_tree(){return "";}
};

class InputsStmt:public Statement{
    char* var_name;
public:
    InputsStmt(std::string stmt,Memory* m,StatementType t);
    ~InputsStmt();
    void get_var_name(char*& v);
    CompVal get_stmt_eval(){return CompVal();}
    //std::vector<CompVal> get_stmt_eval_array(){return {};}
    std::string get_stmt_tree(){return "";};
};

#endif // STATEMENT_H
