#include "statement.h"

StatementType Statement::get_type()
{
    return type;
}


void Statement::get_status(bool& cc,int& pc)
{
    cc=stmt_cc;
    pc=stmt_pc;
}

LetStmt::LetStmt(std::string stmt,Memory* m,StatementType t)
    :Statement(stmt,m,t)
{
    //init parser
    exp=new Parser(m);

    //split the statement to type and exp
    char* str;
    str_to_ptr(stmt,str);
    std::cout<<str<<std::endl;

    char op;
    parse_symbol(str,left_var);
    parse_op(str,op);

    exp->parse_line_wrapper(str);
}

LetStmt::~LetStmt()
{
    delete exp;
}


CompVal LetStmt::get_stmt_eval()
{
    CompVal val;
    exp->eval(val);

    std::string lv(left_var);

    //save to mem ,if var exists just change the value
    if(!mem->mem_search(lv,V_INT)&&
            !mem->mem_search(lv,V_STR))
    {
        mem->mem_add(lv,val);
    }
    else
    {
        mem->mem_replace(lv,val);
    }


    return val;
}

std::string LetStmt::get_stmt_tree()
{
    return "LET = \n"+std::string(left_var)+"\n" +exp->get_syntax_tree();
}



PrintStmt::PrintStmt(std::string stmt,Memory* m,StatementType t)
    :Statement(stmt,m,t)
{
    //init parser
    exp=new Parser(m);

    //split the statement to type and exp
    char* str;
    str_to_ptr(stmt,str);
    std::cout<<str<<std::endl;

    exp->parse_line_wrapper(str);
    //exp->print_syntax_tree();
}

CompVal PrintStmt::get_stmt_eval()
{
    CompVal val;
    exp->eval(val);
    print_val=val;
    return val;
}

PrintStmt::~PrintStmt()
{
    delete exp;
}

std::string PrintStmt::get_stmt_tree()
{
    return "PRINT = \n"+exp->get_syntax_tree();
}

PrintfStmt::PrintfStmt(std::string stmt,Memory* m,StatementType t)
    :Statement(stmt,m,t)
{
    origin_stmt=stmt;
}

/*
 * use this to carry format string
*/
void PrintfStmt::get_var_name(char*& v)
{
    char* str;
    char* str_row;
    argv_num=0;
    str_to_ptr(origin_stmt,str);
    parse_format_string(str,str_row,argv_num);
    row_string=std::string(str_row);
    int tmp_argv=argv_num;
    while(tmp_argv--)
    {
        parse_comma(str);
        SKIP_BLANK(str);
        if(IS_DIGIT(str))
        {
            int val;
            parse_digit(str,val);
            val_array.push_back(CompVal(val));
        }
        else if(IS_LETTER(str))
        {
            char* var;
            parse_symbol(str,var);
            std::string tmp_var=std::string(var);
            CompVal tmp_val;
            if(mem->mem_search(tmp_var,V_STR))
            {
               tmp_val=mem->mem_get(tmp_var,V_STR);
            }
            else if(mem->mem_search(tmp_var,V_INT))
            {
               tmp_val=mem->mem_get(tmp_var,V_INT);
            }
            else
                ErrorHandler::throwMsg(E_UDEF_VAR);
            val_array.push_back(tmp_val);
        }
        else if(IS_STR_DELIM(str))
        {
            char* var_str;
            parse_string(str,var_str);
            val_array.push_back(CompVal(std::string(var_str)));
        }
        else
            ErrorHandler::throwMsg(E_INS_PRINTF);

    }

    //reformat string
    for(auto& val:val_array)
    {
        std::string tmp_str;
        if(val.get_type()==V_STR)
            tmp_str=val.get_str_val();
        else if(val.get_type()==V_INT)
            tmp_str=std::to_string(val.get_int_val());
        size_t pos=row_string.find('{');
        row_string=row_string.replace(pos,2,tmp_str);
        std::cout<<"row string:"<<row_string<<" tmpstr:"<<tmp_str<<std::endl;
    }

    str_to_ptr(row_string,v);
}

PrintfStmt::~PrintfStmt()
{

}

RemStmt::RemStmt(std::string stmt,Memory* m,StatementType t)
    :Statement(stmt,m,t){}

RemStmt::~RemStmt(){}

IfStmt::IfStmt(std::string stmt,Memory* m,StatementType t)
    :Statement(stmt,m,t)
{
    //init parser
    exp1=new Parser(m);
    exp2=new Parser(m);

    char* str;
    str_to_ptr(stmt,str);
    std::cout<<str<<std::endl;

    parse_exp(str,str1);
    parse_cmp_op(str,op);
    parse_exp(str,str2);

    //parse key word
    char* key;
    str_to_ptr("THEN",key);
    parse_keyword(str,key);

    //parse direction
    parse_digit(str,stmt_pc);

    exp1->parse_line_wrapper(str1);
    exp2->parse_line_wrapper(str2);


}

IfStmt::~IfStmt()
{
    delete exp1;
    delete exp2;
}

CompVal IfStmt::get_stmt_eval()
{
    CompVal val1,val2;
    exp1->eval(val1);
    exp2->eval(val2);

    if((op=='>'&&IS_LARGER(val1,val2))
            ||(op=='<'&&IS_LARGER(val2,val1))
            ||(op=='='&&IS_EQUAL(val1,val2)))
    {
        stmt_cc=true;
    }
    else
    {
        stmt_cc=false;
    }
    return 0;
}

std::string IfStmt::get_stmt_tree()
{
    return "IF THEN \n"+exp1->get_syntax_tree()
            +op+'\n'+exp2->get_syntax_tree()+std::to_string(stmt_pc)+'\n';
}

GotoStmt::GotoStmt(std::string stmt,Memory* m,StatementType t)
:Statement(stmt,m,t)
{
    char* str;
    str_to_ptr(stmt,str);

    int to_direc;
    parse_digit(str,to_direc);
    stmt_cc=true;
    stmt_pc=to_direc;
}

GotoStmt::~GotoStmt(){}

std::string GotoStmt::get_stmt_tree()
{
    return "GOTO \n"+std::to_string(stmt_pc)+'\n';
}

EndStmt::EndStmt(std::string stmt,Memory* m,StatementType t)
:Statement(stmt,m,t){}

EndStmt::~EndStmt(){};

InputStmt::InputStmt(std::string stmt,Memory* m,StatementType t)
:Statement(stmt,m,t)
{
    char* str;
    str_to_ptr(stmt,str);
    parse_symbol(str,var_name);
}

void InputStmt::get_var_name(char*& v)
{
    v=var_name;
}

InputStmt::~InputStmt(){};

InputsStmt::InputsStmt(std::string stmt,Memory* m,StatementType t)
:Statement(stmt,m,t)
{
    char* str;
    str_to_ptr(stmt,str);
    parse_symbol(str,var_name);
}

void InputsStmt::get_var_name(char*& v)
{
    v=var_name;
}

InputsStmt::~InputsStmt(){};




