#include "handle.h"

bool parse_symbol(char*& str,char*& name)
{
    SKIP_BLANK(str);
    if(IS_LETTER(str))
    {
        //get symbol
        char* tmp=str;
        int len=0;
        while(!IS_BLANK(tmp)&&!IS_END(tmp)
              &&(IS_LETTER(tmp)||IS_DIGIT(tmp)))
        {
            len++;
            tmp++;
        }
        char* var=new char[len+1];
        var[len]='\0';
        strncpy(var,str,len);
        str+=len;

        name=var;
        return true;
    }
    return false;
}

bool parse_keyword(char*& str,char* key)
{
    SKIP_BLANK(str);
    char* name;
    if(parse_symbol(str,name)
            &&!strcmp(key,name))
    {
        return true;
    }
    return false;
}

bool parse_digit(char*& str,int&val)
{
    SKIP_BLANK(str);
    //handle negative number
    if(*str=='-')
    {
        str++;
        val= -std::strtoul(str, &str, 0);
    }
    else
    {
        val= std::strtoul(str, &str, 0);
    }

    return true;
}

bool parse_op(char*& str,char& op)
{
    SKIP_BLANK(str);
    op=*str;
    if(IS_OPERATOR(str))
    {
        str++;
        return true;
    }
    return false;
}

bool parse_cmp_op(char*& str,char& op)
{
    SKIP_BLANK(str);
    op=*str;
    if(IS_CMP_OP(str))
    {
        str++;
        return true;
    }
    return false;
}

//exp and THEN must have blank
bool parse_exp(char*& str,char*& exp)
{
    int len=0;
    SKIP_BLANK(str);
    char* tmp=str;
    while(!IS_CMP_OP(tmp)&&!IS_END(tmp)&&!IS_KEY_THEN(tmp))
    {
        tmp++;
        len++;
    }
    exp=new char[len+1];
    exp[len]='\0';
    strncpy(exp,str,len);
    str+=len;
    return true;
}

bool parse_string(char*& str,char*& str_after)
{
    int len=0;
    char* left_delim,*right_delim;
    char* row_begin;
    SKIP_BLANK(str);
    if(IS_STR_DELIM(str))
    {
        left_delim=str;
        str++;
    }
    else
        return false;

    row_begin=str;

    while(!IS_END(str)&&!IS_STR_DELIM(str))
    {
        len++;
        str++;
    }

    //if right delimiter missing
    if(!IS_STR_DELIM(str))
        return false;
    right_delim=str;

    //skip blanks
//    SKIP_BLANK(str);

//    //if string still contains char, it's invalid
//    if(!IS_END(str))
//        return false;

    if(!IS_DELIM_MATCH(left_delim,right_delim))
        return false;

    //correct string
    str_after=new char[len+1];
    str_after[len]='\0';
    strncpy(str_after,row_begin,len);

    return true;
}

bool str_to_ptr(std::string src,char*& dest)
{
    dest=new char[src.length()+1];
    dest[src.length()]='\0';
    strcpy(dest,src.c_str());
    return true;
}

