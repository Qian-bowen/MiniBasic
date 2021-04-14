#include "program.h"

void Program::run(std::list<int> args_value)
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
            int result=it->second->get_stmt_eval();
            result_buf+=std::to_string(result)+'\n';
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
            //put sth into mem
            char* var_name;
            it->second->get_var_name(var_name);
            mem.insert(std::pair<std::string,int>(std::string(var_name),*args_it));
            ++args_it;
            break;
        }

        //set pc according to cc
        it->second->get_status(stat->cc,stat->pc);
        if(stat->cc)
        {
            it=prog.find(stat->pc);
            if(it==prog.end()) throw("INVALID GOTO ADDRESS");
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
    //clear mem
    mem.clear();
    //clear prog
    prog.clear();
    //reset buffer
    result_buf="";
    tree_buf="";
    //reset args num
    args_num=0;

    if(buffer.empty()) throw("PLEASE ENTER PROGRAM");

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

Program::~Program()
{
    if(stat!=NULL) delete stat;
}
