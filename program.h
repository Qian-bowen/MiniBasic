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
    std::unordered_map<std::string,int> mem;
    status* stat;

    std::map<int,Statement*> prog;//store
    int args_num;//input args of program

    std::string result_buf,tree_buf;

public:
    Program(){}
    ~Program();
    int load_into_prog(QList<Line> buffer);//return args nums
    void run(std::list<int> args_value);
    std::string get_result_buf(){return result_buf;}
    std::string get_tree_buf(){return tree_buf;}
    void clear_program(){prog.clear();}


};

#endif // PROGRAM_H
