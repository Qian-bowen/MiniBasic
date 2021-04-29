#include "program.h"

void Program::reset_program()
{
    //clear mem
    mem.mem_clear();
    //clear prog
    prog.clear();
    //reset buffer
    result_buf="";
    tree_buf="";
    //reset args num
    args_num=0;
}

void Program::run(std::list<CompVal> args_value)
{

    auto it=prog.begin();
    auto args_it=args_value.begin();
    while(it!=prog.end())
    {
        StatementType t=it->second->get_type();
        if(t==END) break;//end the program if meet END

        switch(t)
        {
        case LET:
            it->second->get_stmt_eval();
            break;
        case PRINT:
        {
            CompVal result=it->second->get_stmt_eval();
            if(result.get_type()==V_INT)
            {
                result_buf+=std::to_string(result.get_int_val())+'\n';
            }
            else if(result.get_type()==V_STR)
            {
                result_buf+=result.get_str_val()+'\n';
            }
            break;
        }
        case REM:
            break;
        case IF:
            it->second->get_stmt_eval();
            break;
        case GOTO:
            break;
        case INPUT:
        case INPUTS:
            //put sth into mem
            char* var_name;
            it->second->get_var_name(var_name);
            mem.mem_add(var_name,*args_it);
            ++args_it;
            break;
        default:
            break;
        }

        //set pc according to cc
        it->second->get_status(stat->cc,stat->pc);
        if(stat->cc)
        {
            it=prog.find(stat->pc);
            if(it==prog.end()) ErrorHandler::throwMsg(E_IVD_ADDR);
        }
        else
        {
            ++it;
        }
        stat->cc=false;
    }

    //reset status after the program end
    stat->pc=-1;
}

int Program::load_into_prog(QList<Line> buffer)
{
    reset_program();

    if(buffer.empty()) ErrorHandler::throwMsg(E_PRO_MISS);

    for(auto it=buffer.begin();it!=buffer.end();++it)
    {
        StatementType cur_type=it->type;
        Statement* s;
        switch(cur_type)
        {
        case LET:
            s=new LetStmt((*it).content,&mem,cur_type);
            break;
        case PRINT:
            s=new PrintStmt((*it).content,&mem,cur_type);
            break;
        case REM:
            s=new RemStmt((*it).content,&mem,cur_type);
            break;
        case IF:
            s=new IfStmt((*it).content,&mem,cur_type);
            break;
        case GOTO:
            s=new GotoStmt((*it).content,&mem,cur_type);
            break;
        case END:
            s=new EndStmt((*it).content,&mem,cur_type);
            break;
        case INPUT:
            s=new InputStmt((*it).content,&mem,cur_type);
            ++args_num;
            break;
        case INPUTS:
            s=new InputsStmt((*it).content,&mem,cur_type);
            ++args_num;
            break;
        default:
            break;
        }

        prog.insert(std::pair<int,Statement*>((*it).num,s));
        if(cur_type==LET||cur_type==PRINT||cur_type==IF||cur_type==GOTO)
        {
            std::string line_num=std::to_string((*it).num);
            tree_buf+=line_num+' '+s->get_stmt_tree();
        }
    }

    //init stat
    auto first=buffer.begin();
    stat=new status(first->num);
    return args_num;
}

void Program::mem_add_prog(std::string var,CompVal val)
{
    if(mem.mem_search(var,val.get_type()))
    {
        mem.mem_replace(var,val);
    }
    else
        mem.mem_add(var,val);
}

void Program::clear_program()
{
    mem.mem_clear();
    prog.clear();
}

Program::~Program()
{
    if(stat!=NULL) delete stat;
}
