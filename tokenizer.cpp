#include "tokenizer.h"

void Tokenizer::token_convert(std::string str)
{
    char* p=&str[0];
    //get statement type
    std::string stmt_str="";
    while(*p!=' ')
    {
        stmt_str+=*p;
        ++p;
    }
    stmt_type=stmt_str;

    //get other tokens
    while(*p!='\0')
    {
        std::string exp_str="";
        while(*p==' ')
        {
            ++p;
        }
        exp_token.push_back(exp_str);
        ++p;
    }
}
