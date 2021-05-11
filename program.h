#ifndef PROGRAM_H
#define PROGRAM_H

#include "statement.h"
#include<list>
#include<map>
#include<string>
#include<QList>
#include "line.h"


class Program{
private:
    Memory mem;
    status* stat;

    std::map<int,Statement*> prog;
    int args_iter;
    std::vector<CompVal> argvs;

    std::string result_buf,tree_buf;

    void reset_program();
    std::string format_string(std::string origin_str,std::vector<CompVal> vc);
    std::map<int,Statement*>::iterator exec_one_statement(int cur_pc);

public:
    Program(){}
    ~Program();
    int load_into_prog(QList<Line> buffer);//return args nums
    bool is_program_empty(){return prog.empty();}
    void run(std::vector<CompVal> args_value);
    bool run_one_step();

    void load_args_value(std::vector<CompVal> args_value);
    void mem_add_prog(std::string var,CompVal val);
    bool get_mem_value(std::string var,CompVal& val);
    std::string get_result_buf(){return result_buf;}
    std::string get_tree_buf(){return tree_buf;}
    std::string get_current_tree();

    std::string prog_snapshot();
    Memory* mem_snapshot(){return &mem;}

    int get_pc(){return stat->pc;}
    void clear_program();

};

#endif // PROGRAM_H
