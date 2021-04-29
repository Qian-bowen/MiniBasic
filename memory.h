#ifndef MEMORY_H
#define MEMORY_H
#include<unordered_map>
#include "handle.h"
#include "variable.h"

class Memory{
private:
    std::unordered_map<std::string,CompVal> memory;

public:
    void mem_add(std::string var,CompVal val);
    void mem_replace(std::string var,CompVal val);
    bool mem_search(std::string var,VarType type);
    CompVal mem_get(std::string var,VarType type);
    void mem_clear();
};


#endif // MEMORY_H