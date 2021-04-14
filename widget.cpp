#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    ui->setupUi(this);
    this->resize(1200,2000);
    layout=new QVBoxLayout(this);
    code_result_layout=new QHBoxLayout;
    button_layout=new QHBoxLayout;
    code_div=new QTextBrowser;
    cmd_div=new QLineEdit;
    result_div=new QTextBrowser;
    tree_div=new QTextBrowser;
    load_but=new QPushButton("LOAD");
    run_but=new QPushButton("RUN");
    clear_but=new QPushButton("CLEAR");


    layout->addLayout(code_result_layout);
    layout->addLayout(button_layout);
    layout->addWidget(new QLabel(tr("语句与语法树")));
    layout->addWidget(tree_div);
    layout->addWidget(new QLabel(tr("命令输入窗口")));
    layout->addWidget(cmd_div);

    QGroupBox* code_box=new QGroupBox(tr("代码"));
    QVBoxLayout* code_layout=new QVBoxLayout;
    code_layout->addWidget(code_div);
    code_box->setLayout(code_layout);

    QGroupBox* result_box=new QGroupBox(tr("结果"));
    QVBoxLayout* result_layout=new QVBoxLayout;
    result_layout->addWidget(result_div);
    result_box->setLayout(result_layout);


    code_result_layout->addWidget(code_box);
    code_result_layout->addWidget(result_box);
    button_layout->addWidget(load_but);
    button_layout->addWidget(run_but);
    button_layout->addWidget(clear_but);

    connect(load_but,SIGNAL(clicked()),this,SLOT(openFile()));
    connect(run_but,SIGNAL(clicked()),this,SLOT(runCode()));
    connect(clear_but,SIGNAL(clicked()),this,SLOT(clearProgram()));
    connect(cmd_div,SIGNAL(returnPressed()),this,SLOT(getCmd()));
    connect(cmd_div,SIGNAL(returnPressed()),cmd_div,SLOT(clear()));
    connect(this,SIGNAL(run_complete()),this,SLOT(displayRltTree()));
}

void Widget::storeCmd(QString cur_line)
{
    int cur_num;
    char* str;
    StatementType cur_type;
    str_to_ptr(cur_line.toStdString(),str);

    //handle input cmd
    if(str[0]=='?')
    {
        str++;
        parse_digit(str,cur_num);
        std::cout<<"?:"<<cur_num<<std::endl;
        args_value.push_back(cur_num);
        return;
    }

    //parse line num
    SKIP_BLANK(str);
    if(!IS_DIGIT(str)) throw("LINE NUMBER MISSING\n");
    parse_digit(str,cur_num);

    //line without number
    SKIP_BLANK(str);
    std::string lwn=std::string(str);

    //delete a line
    auto itp=std::find(buffer.begin(),buffer.end(),cur_num);
    if(itp!=buffer.end()&&lwn.size()==0)
    {
        buffer.erase(itp);
        return;
    }

    //parse statement type
    char* name;
    parse_symbol(str,name);
    std::string head_type(name);

    //use regular expression to fliter invalid instruction
    QString var="[a-zA-Z][0-9a-zA-Z]*";//var cannot begin with digit
    QString exp="[0-9a-zA-Z\\s+\\-*/()]+";
    QString address="[0-9]+";
    QString cmp_op="[><=]";

    if(!strcmp(name,"LET"))
    {
        QRegExp let("^\\s*"+var+"\\s*=\\s*"+exp+"\\s*$");
        if(!let.exactMatch(QString(str)))
            throw("INVALID INSTRUCTION LET");
        cur_type=LET;
    }
    else if(!strcmp(name,"PRINT"))
    {
        QRegExp print("^\\s*"+exp+"\\s*$");
        if(!print.exactMatch(QString(str)))
            throw("INVALID INSTRUCTION PRINT");
        cur_type=PRINT;
    }
    else if(!strcmp(name,"GOTO"))
    {
        QRegExp go_to("^\\s*"+address+"\\s*$");
        if(!go_to.exactMatch(QString(str)))
            throw("INVALID INSTRUCTION GOTO");
        cur_type=GOTO;
    }
    else if(!strcmp(name,"REM"))
    {
        cur_type=REM;
    }
    else if(!strcmp(name,"END"))
    {
        QRegExp end("^\\s*$");
        if(!end.exactMatch(QString(str)))
            throw("INVALID INSTRUCTION END");
        cur_type=END;
    }
    else if(!strcmp(name,"IF"))
    {
        QRegExp ifreg("^\\s*"+exp+"\\s*"+cmp_op+"\\s*"+exp
                      +"\\s*"+"THEN"+"\\s*"+address+"\\s*$");
        if(!ifreg.exactMatch(QString(str)))
            throw("INVALID INSTRUCTION IF");
        cur_type=IF;
    }
    else if(!strcmp(name,"INPUT"))
    {
        QRegExp input("^\\s*"+var+"\\s*$");
        if(!input.exactMatch(QString(str)))
            throw("INVALID INSTRUCTION INPUT");
        cur_type=INPUT;
    }
    else
    {
        throw("INSTRUCTION NOT EXIST");
    }

    //skip all the blank left
    SKIP_BLANK(str);
    Line line(cur_num,std::string(str),head_type,cur_type);
    std::cout<<"line size:"<<line.content.size()<<std::endl;

    //replace a line if valid
    auto it=std::find(buffer.begin(),buffer.end(),line.num);
    if(it!=buffer.end()&&lwn.size()>0)
    {
        buffer.erase(it);
    }

    buffer.push_back(line);

    //sort list by line num
    std::sort(buffer.begin(),buffer.end(),
              [](const Line& l1,const Line& l2)->bool{return l1.num<l2.num;});
}

void Widget::storeCmdWrapper(QString cur_line)
{
    try {
        storeCmd(cur_line);
    }  catch (const char* s) {
        std::cout<<s<<std::endl;
        result_div->append(QString(s));
        return;
    }
}

void Widget::showProgm()
{
    QString s="";
    for(auto it=buffer.begin();it!=buffer.end();++it)
    {
        QString tmp=QString((*it).head.data())+' '+QString((*it).content.data());
        s+=QString::number((*it).num)+' '+tmp+'\n';
    }
    code_div->setPlainText(s);
}


void Widget::openFile()
{
    vmline=0;
//    buffer.clear();
//    args_value.clear();

    QString file_name = QFileDialog::getOpenFileName(NULL,"选择文件",".","*.txt");
    QFile file(file_name);
    file.open(QIODevice::ReadOnly);
    QTextStream out_byte(&file);
    QString cur_line;

    //form the program list
    while(out_byte.readLineInto(&cur_line))
    {
        if(cur_line.isEmpty()) continue;
        storeCmdWrapper(cur_line);
    }

    file.close();
    showProgm();

}

void Widget::getCmd()
{
    QString cmd=cmd_div->text();
    //PARSE CMD
    if(cmd=="QUIT")
    {
        exit(0);
    }
    else if(cmd=="CLEAR")
    {
        clearProgram();
    }
    else if(cmd=="RUN")
    {
        runCode();
    }
    else if(cmd=="LOAD")
    {
        openFile();
    }
    else if(cmd=="LIST")
    {
        //do nothing
    }
    else if(cmd=="HELP")
    {
        code_div->setText("HELP INFOMATION");
    }
    //let\input\print without
    else if(cmd.startsWith("LET"))
    {
        std::string num=std::to_string(vmline);
        QString s(num.data());
        QString cmd_tmp=s+" "+cmd;
        vmline++;
        storeCmdWrapper(cmd_tmp);
        showProgm();

    }
    else if(cmd.startsWith("INPUT"))
    {

        showPrompt();
        QEventLoop loop;
        connect(cmd_div,SIGNAL(returnPressed()),&loop,SLOT(quit()));
        loop.exec();

        std::string num=std::to_string(vmline);
        QString s(num.data());
        QString cmd_tmp=s+" "+cmd;
        vmline++;
        storeCmdWrapper(cmd_tmp);
        showProgm();
        return;
    }
    else if(cmd.startsWith("PRINT"))
    {
        std::string num=std::to_string(vmline);
        QString s(num.data());
        QString cmd_tmp=s+" "+cmd;
        vmline++;
        storeCmdWrapper(cmd_tmp);
        showProgm();
        runCode();
    }
    else
    {
        storeCmdWrapper(cmd);
        showProgm();
    }
}

void Widget::showPrompt()
{
    cmd_div->clear();
    cmd_div->insert(QString("? "));
}

void Widget::runCode()
{
    int args_num;
    try {
        args_num=program.load_into_prog(buffer);
    }  catch (char const* s) {
        std::cout<<s<<std::endl;
        result_div->append(QString(s));
        return;
    }


    //get args from cmd line
    //todo qeventloop
    int remain_args=args_num-args_value.size();
    while(remain_args--)
    {
        showPrompt();
        QEventLoop loop;
        connect(cmd_div,SIGNAL(returnPressed()),&loop,SLOT(quit()));
        loop.exec();
    }
    //run the program
    try {
        program.run(args_value);
    }
    catch (char const* s)
    {
        std::cout<<s<<std::endl;
        result_div->append(QString(s));
        program.clear_program();
        return;
    }

    emit run_complete();
    //clear program
    program.clear_program();
    args_value.clear();
}


/*
* clear buffer and program
*/
void Widget::clearProgram()
{
    //delete program
    result_div->clear();
    tree_div->clear();
    code_div->clear();
    cmd_div->clear();
    //todo clear buffer
    buffer.clear();
    args_value.clear();
    //reset vmline
    vmline=0;

}

void Widget::displayRltTree()
{
    result_buf="";
    tree_buf="";

    result_buf=QString(program.get_result_buf().data());
    tree_buf=QString(program.get_tree_buf().data());
    result_div->clear();
    tree_div->clear();
    result_div->setText(result_buf);
    tree_div->setText(tree_buf);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::buffer_out()
{
    for(auto it=buffer.begin();it!=buffer.end();++it)
    {
        std::cout<<(*it).content<<std::endl;
    }
}



