#ifndef PROGRAM_H
#define PROGRAM_H

#include "statement.h"
#include<list>
#include<map>
#include<string>
#include<functional>
#include<QList>
#include<QObject>
#include "line.h"


class Program:public QObject{
    Q_OBJECT
private:
    Memory mem;
    status* stat;

    std::map<int,Statement*> prog;
    int args_iter;
    std::vector<CompVal> argvs;

    void reset_program();
    std::string format_string(std::string origin_str,std::vector<CompVal> vc);
    std::map<int,Statement*>::iterator exec_one_statement(int cur_pc);

    void run();
    bool run_one_step();

signals:
    void get_input(std::string var_name,StatementType type);
    void result_print(std::string result);
    void tree_print(std::string tree);

public slots:
     bool runWrapper(Mode mode);

public:
    Program(QObject* parent = NULL);
    ~Program();
    int load_into_prog(QList<Line> buffer);//return args nums
    bool is_program_empty(){return prog.empty();}


    void mem_add_prog(std::string var,CompVal val);
    bool get_mem_value(std::string var,CompVal& val);
    std::string get_current_tree();

    std::string prog_snapshot();
    Memory* mem_snapshot(){return &mem;}

    int get_pc(){return stat->pc;}
    void clear_program();

};

#endif // PROGRAM_H
