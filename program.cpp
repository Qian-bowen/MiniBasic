#include "program.h"

Program::Program(QObject* parent)
    :QObject(parent)
{

}

void Program::reset_program()
{
    //clear mem
    mem.mem_clear();
    //clear prog
    prog.clear();
    //clear argument
    argvs.clear();
    //reset args num
    args_iter=0;
}

std::map<int,Statement*>::iterator Program::exec_one_statement(int cur_pc)
{
    auto it=prog.find(cur_pc);
    if(it==prog.end()) ErrorHandler::throwMsg(E_IVD_ADDR);

    auto pre_it=it;

    StatementType t=it->second->get_type();
    //if meet end ,return the sign of end
    switch(t)
    {
    case LET:
        it->second->get_stmt_eval();
        break;
    case IF:
        it->second->get_stmt_eval();
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

    //std::cout<<"set pc:"<<stat->pc<<std::endl;


    if(t==INPUT||t==INPUTS)
    {
        char* input_var_name;
        pre_it->second->get_var_name(input_var_name);
        emit get_input(input_var_name,t);
        //suspend program
        throw(STALL);
    }
    else if(t==PRINT)
    {
        CompVal result=pre_it->second->get_stmt_eval();
        if(result.get_type()==V_INT)
        {
            emit result_print(std::to_string(result.get_int_val()));
        }
        else if(result.get_type()==V_STR)
        {
            emit result_print(result.get_str_val());
        }
    }
    else if(t==PRINTF)
    {
        char* printf_result;
        pre_it->second->get_var_name(printf_result);
        emit result_print(std::string(printf_result));
    }

    return it;
}

/*
 * according to mode eo exe run
*/
bool Program::runWrapper(Mode mode)
{
    if(mode==RUN)
    {
        run();
    }
    else if(mode==DEBUG)
    {
        return run_one_step();
    }
    return true;
}

void Program::run()
{
    //run from current pc
    auto it=prog.find(stat->pc);
    while(it!=prog.end())
    {
        int cur_pc=stat->pc;
        try {
            it=exec_one_statement(stat->pc);
        }  catch (const char* msg) {
            RunError runError(cur_pc,msg);
            throw(runError);
        }catch(PROG_MODE mode){
            if(mode==STALL)
                return;
        }
    }

    //reset status after the program end
    stat->pc=-1;
}

/*
 * run one step
 * return false if no statement left to run
 * if meet with error throw it immediately
*/
bool Program::run_one_step()
{
    std::map<int,Statement*>::iterator it;
    int cur_pc=stat->pc;
    try {
        it=exec_one_statement(stat->pc);
    }  catch (const char* msg) {
        RunError runError(cur_pc,msg);
        throw(runError);
    }   catch(PROG_MODE mode){
        if(mode==STALL)
            return true;
    }

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
            //++args_num;
            break;
        case INPUTS:
            s=new InputsStmt((*it).content,&mem,cur_type);
            //++args_num;
            break;
        default:
            break;
        }

        prog.insert(std::pair<int,Statement*>((*it).num,s));

        std::string line_num=std::to_string((*it).num);
        emit tree_print(line_num+' '+s->get_stmt_tree()+'\n');
    }

    //init stat
    auto first=buffer.begin();
    stat=new status(first->num);
    return args_num;
}

void Program::mem_add_prog(std::string var,CompVal val)
{
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
