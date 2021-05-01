#ifndef PROGRAM_H
#define PROGRAM_H

#include "statement.h"
#include<list>
#include<map>
#include<string>
#include<QList>
#include "line.h"

struct status{
    bool cc;//condition code, false means run next line
    int pc;//next line to execute
    status(int p):cc(false),pc(p){}
};

class Program{
private:
    Memory mem;
    status* stat;

    std::map<int,Statement*> prog;//store
    int args_num;//input args of program

    std::string result_buf,tree_buf;

    void reset_program();
    std::string format_string(std::string origin_str,std::vector<CompVal> vc);

public:
    Program(){}
    ~Program();
    int load_into_prog(QList<Line> buffer);//return args nums
    void run(std::list<CompVal> args_value);
    void mem_add_prog(std::string var,CompVal val);
    bool get_mem_value(std::string var,CompVal& val);
    std::string get_result_buf(){return result_buf;}
    std::string get_tree_buf(){return tree_buf;}
    std::string prog_snapshot();
    void clear_program();



};

#endif // PROGRAM_H
