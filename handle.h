#ifndef HANDLE_H
#define HANDLE_H
#include <string>
#include <string.h>
#include <iostream>//test
#include <stdlib.h>


#define IS_BLANK(s) ((*s)==' '||(*s)=='\t')
#define IS_DIGIT(s) ((*s)>='0'&&(*s)<='9')
#define IS_LETTER(s) (((*s)>='a'&&(*s)<='z')||((*s)>='A'&&(*s)<='Z'))
#define IS_FONT_BRACKET(s) ((*s)=='(')
#define IS_BACK_BRACKET(s) ((*s)==')')
//all operator
#define IS_OPERATOR(s) ((*s)=='+'||(*s)=='-'||(*s)=='*'||(*s)=='/'||(*s)=='=')
#define IS_CMP_OP(s) ((*s)=='='||(*s)=='>'||(*s)=='<')
#define IS_END(s) (*(s)=='\0')
#define SKIP_BLANK(s) do { \
  while(!IS_END(s) && IS_BLANK(s)) \
    (s)++; \
} while(0);

#define IS_LARGER(a,b) (a>b)
#define IS_EQUAL(a,b) (a==b)

#define IS_STR_DELIM(s) ((*s)=='\''||(*s)=='"')
#define IS_DELIM_MATCH(a,b) (((*a=='\'')&&(*b=='\''))||((*a=='"') && (*b=='"')))

//if precedence of a is higher or equal then b,return true
//handle power in parse_line
#define IS_PRECE_HIGH_EQU(a,b) (((a=='*'||a=='/')&&(b=='+'||b=='-')) \
    ||((a=='+'||a=='-')&&(b=='+'||b=='-')) \
    ||((a=='*'||a=='/')&&(b=='*'||b=='/')))

//whether s contain a prefex THEN
#define IS_KEY_THEN(s) ((*s)=='T'&&(*(s+1))=='H'&&(*(s+2))=='E'&&(*(s+3))=='N')

bool parse_symbol(char*& str,char*& name);
bool parse_keyword(char*& str,char* key);//check symbol whether equal the key
bool parse_digit(char*& str,int&val);
bool parse_op(char*& str,char& op);
bool parse_cmp_op(char*& str,char& op);
bool parse_exp(char*& str,char*& exp);
bool parse_string(char*& str,char*& str_after);
bool str_to_ptr(std::string src,char*& dest);


#endif // HANDLE_H
