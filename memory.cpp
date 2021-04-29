#include "memory.h"

void Memory::mem_add(std::string var,CompVal val)
{
    auto tmp=std::pair<std::string,CompVal>(var,val);
    memory.insert(tmp);
}

void Memory::mem_replace(std::string var,CompVal val)
{
    auto search=memory.find(var);
    search->second=val;
}

/*
search variable in memory
if not exist add it
*/
bool Memory::mem_search(std::string var,VarType type)
{
    auto search=memory.find(var);
    //not find variable
    if(search==memory.end())
        return false;
    if(type!=(*search).second.get_type())
        return false;
    return true;
}

CompVal Memory::mem_get(std::string var,VarType type)
{
    auto search=memory.find(var);
    return search->second;
}

void Memory::mem_clear()
{
    memory.clear();
}
