#include "parser.h"

void Parser::calculate_two_op()
{
    node* op=optor.top();
    optor.pop();

    node* right_op=operands.top();
    operands.pop();
    node* left_op=operands.top();
    operands.pop();


    op->left=left_op;
    op->right=right_op;

    operands.push(op);
}

void Parser::eval_tree(node* root,int& val,bool& succ)
{
    if(root==NULL) return;

    if(root->type==CONSTANT||root->type==VIRTUAL)
    {
        std::cout<<"value:"<<root->value<<std::endl;
        val=root->value;
        return;
    }
    if(root->type==IDENTIFIER)
    {
        auto search=mem->find(root->content);
        if(search!=mem->end())
        {
            root->value=search->second;
            val=search->second;
            return;
        }
        else
        {
            throw("UNDEFINED VAR");
        }
        succ=false;
        return;
    }

    //compound
    char op=root->op;
    int left_val,right_val;

    eval_tree(root->left,left_val,succ);
    eval_tree(root->right,right_val,succ);

    if(op=='+')
    {
        val=left_val+right_val;
    }
    else if(op=='-')
    {
        val=left_val-right_val;
    }
    else if(op=='*')
    {
        val=left_val*right_val;
    }
    else if(op=='/')
    {
        if(right_val==0) throw("DIVIDE BY ZERO");
        val=left_val/right_val;
    }
    else if(op=='^')
    {
        val=pow(left_val,right_val);
    }
    else
    {
        succ=false;
        return;
    }

    //root->type=CONSTANT;
    root->value=val;
    return;
}

bool Parser::eval(int& val)
{
    bool succ=true;
    int exp_val;
    eval_tree(root,exp_val,succ);
    if(succ)
    {
        val=exp_val;
        return true;
    }
    return false;
}

void Parser::parse_line(std::string exp_str)
{
    char* str;
    str_to_ptr(exp_str,str);
    SKIP_BLANK(str);

    //handle negative eception
    //virtual node is node with value 0
    //handle situation: -5+2, 2+(-2+8)
    if(*str=='-')
    {
        node* vir_node=new node();
        vir_node->type=VIRTUAL;
        vir_node->value=0;
        operands.push( vir_node);
    }


   while(!IS_END(str))
    {
        node* n=new node;
        SKIP_BLANK(str);
        char* t=str;++t;

        if(IS_DIGIT(str))
        {
            int val;
            parse_digit(str,val);
            n->type=CONSTANT;
            n->value=val;
            operands.push(n);
            continue;
        }
        if(IS_LETTER(str))
        {
            char* var;
            parse_symbol(str,var);

            n->type=IDENTIFIER;
            n->content=std::string(var);
            operands.push(n);

            continue;
        }
        //power operator
        //PAY ATTENTINO TO THE ORDER! HANDLE POWER BEFORE OTHER OPERATIONS
        if((*str=='*')&&!(IS_END(t))&&(*t=='*'))
        {
            n->type=OPERATOR;
            n->op='^';
            while(!optor.empty() && IS_PRECE_HIGH_EQU(optor.top()->op,*str))
            {
                calculate_two_op();
            }

            str+=2;
            optor.push(n);
            continue;
        }
        if(IS_OPERATOR(str))
        {
            n->type=OPERATOR;
            n->op=*str;
            while(!optor.empty() &&
                  (IS_PRECE_HIGH_EQU(optor.top()->op,*str)||optor.top()->op=='^'))
            {
                calculate_two_op();
            }

            str++;
            optor.push(n);
            continue;
        }
        if(IS_FONT_BRACKET(str))
        {
            n->op=*str;
            str++;
            optor.push(n);

            //handle negative exception
            SKIP_BLANK(str);
            if(*str=='-')
            {
                node* vir_node=new node();
                vir_node->type=VIRTUAL;
                vir_node->value=0;
                operands.push( vir_node);
            }

            continue;
        }
        if(IS_BACK_BRACKET(str))
        {
            while(optor.top()->op!='(')
            {
                calculate_two_op();
            }
            //pop font bracket
            optor.pop();
            str++;
            continue;
        }
    }

    while(!optor.empty()&&operands.size()>1)
    {
        calculate_two_op();
    }
    if(!optor.empty())
    {
        throw("INVALID EXPRESSION");
    }

    root=operands.top();
}

void Parser::parse_line_wrapper(std::string exp_str)
{
    parse_line(exp_str);
}

std::string Parser::space_create(int level)
{
    std::string tmp="  ";//DEFAULT SPACE*2
    for(int i=0;i<level;++i)
    {
        tmp+="  ";
    }
    return tmp;
}

std::string Parser::print_node(node* cur,int level)
{
    std::string tmp="";
    switch(cur->type)
    {
    case CONSTANT:
        tmp+=space_create(level);
        tmp+=std::to_string(cur->value)+'\n';
        break;
    case IDENTIFIER:
        tmp+=space_create(level);
        tmp+=cur->content+'\n';
        break;
    case OPERATOR:
        tmp+=space_create(level);
        if(cur->op=='^')
            tmp+="**\n";
        else
            tmp+=std::string(1,cur->op)+"\n";
        std::cout<<"op:"<<cur->op<<std::endl;
        break;
    }
    return tmp;
}



std::string Parser::middle_travel(node* root,int level)
{
    if(root!=NULL)
    {
        return (print_node(root,level)+
        middle_travel(root->left,level+1)+
        middle_travel(root->right,level+1));
    }
    return "";
}


std::string Parser::get_syntax_tree()
{
    return middle_travel(root,0);
}

