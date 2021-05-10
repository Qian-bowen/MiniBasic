#include "program.h"

void Program::reset_program()
{
    //clear mem
    mem.mem_clear();
    //clear prog
    prog.clear();
    //clear argument
    argvs.clear();
    //reset buffer
    result_buf="";
    tree_buf="";
    //reset args num
    args_iter=0;
}

std::map<int,Statement*>::iterator Program::exec_one_statement(int cur_pc)
{
    auto it=prog.find(cur_pc);
    if(it==prog.end()) ErrorHandler::throwMsg(E_IVD_ADDR);

    StatementType t=it->second->get_type();
    //if meet end ,return the sign of end
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
    case PRINTF:
    {
        char* result;
        it->second->get_var_name(result);
        result_buf+=std::string(result)+'\n';
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
        mem.mem_add(var_name,argvs[args_iter]);
        ++args_iter;
        break;
    case END:
        return prog.end();
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

    //set pc and cc
    stat->cc=false;
    stat->pc=it->first;
    return it;
}

void Program::run(std::vector<CompVal> args_value)
{
    argvs=args_value;

    auto it=prog.begin();
    while(it!=prog.end())
    {
        it=exec_one_statement(stat->pc);
    }

    //reset status after the program end
    stat->pc=-1;
}

/*
 * run one step
 * return false if no statement left to run
*/
bool Program::run_one_step()
{
    auto it=exec_one_statement(stat->pc);
    if(it==prog.end())
        return false;
    return true;
}

int Program::load_into_prog(QList<Line> buffer)
{
    reset_program();
    int args_num=0;

    if(buffer.empty()) return -1;

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
        case PRINTF:
            s=new PrintfStmt((*it).content,&mem,cur_type);
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

bool Program::get_mem_value(std::string var,CompVal& val)
{
    if(mem.mem_search(var,V_INT))
    {
        val=mem.mem_get(var,V_INT);
        return true;
    }
    if(mem.mem_search(var,V_STR))
    {
        val=mem.mem_get(var,V_STR);
        return true;
    }
    return false;
}

std::string Program::get_current_tree()
{
    auto it=prog.find(stat->pc);
    if(it==prog.end()) ErrorHandler::throwMsg(E_IVD_ADDR);
    return it->second->get_stmt_tree();
}

/*
 * get current status of the program
*/
std::string Program::prog_snapshot()
{
    std::string info="";
    std::string type_int="INT";
    std::string type_str="STR";
    std::unordered_map<std::string,CompVal> mem_slice=mem.mem_snapshot();
    for(auto& var:mem_slice)
    {
        std::string val;
        info+=var.first+":";
        if(var.second.get_type()==V_INT)
        {
            info+=type_int;
            val=std::to_string(var.second.get_int_val());
        }
        if(var.second.get_type()==V_STR)
        {
            info+=type_str;
            val=var.second.get_str_val();
        }
        info+="="+val+"\n";
    }
    return info;
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
