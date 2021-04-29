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

void Widget::highlightLine(int line,QColor color,QList<QTextEdit::ExtraSelection>& extras)
{
    QTextEdit::ExtraSelection h;
    QTextBrowser* code=code_div;
    QTextBlock line_block=code->document()->findBlockByLineNumber(line);
    QTextCursor cursor(line_block);
    h.cursor=cursor;
    h.format.setProperty(QTextFormat::FullWidthSelection,true);
    h.format.setBackground(color);
    extras.append(h);
}

void Widget::highlightAll(QList<int>& error_hlt,QList<int>& debug_hlt)
{
    QList<QTextEdit::ExtraSelection> extras;
    QColor debug_green(100, 255, 100);
    QColor error_red(255, 100, 100);
    for(auto& error:error_hlt)
    {
        highlightLine(error,error_red,extras);
    }
    for(auto& debug:debug_hlt)
    {
        highlightLine(debug,debug_green,extras);
    }
    code_div->setExtraSelections(extras);
}

void Widget::unhighlightAll(QList<int>& error_hlt,QList<int>& debug_hlt)
{
    QList<QTextEdit::ExtraSelection> extras;
    QColor uncolor_write(0,0, 255);
    for(auto& error:error_hlt)
    {
        highlightLine(error,uncolor_write,extras);
    }
    for(auto& debug:debug_hlt)
    {
        highlightLine(debug,uncolor_write,extras);
    }
    code_div->setExtraSelections(extras);
}


void Widget::storeCmd(QString cur_line)
{
    Error cmd_error=E_NONE;
    int cur_num;
    char* str;
    StatementType cur_type;
    str_to_ptr(cur_line.toStdString(),str);

    SKIP_BLANK(str);

    //handle prompt
    if(*str=='?')
    {
        str++;
        SKIP_BLANK(str);
        if(IS_DIGIT(str))
        {
            int val;
            parse_digit(str,val);
            args_value.push_back(CompVal(val));
        }
        else if(IS_STR_DELIM(str))
        {
            char* val;
            parse_string(str,val);
            args_value.push_back(CompVal(std::string(val)));
        }
        return;
    }

     //parse line num
    if(!IS_DIGIT(str))
    {
        cmd_error=E_LINE_MISS;
    }
    else
    {
        parse_digit(str,cur_num);
    }

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

    if(!strcmp(name,"LET"))
    {
        cur_type=LET;
    }
    else if(!strcmp(name,"PRINT"))
    {
        cur_type=PRINT;
    }
    else if(!strcmp(name,"GOTO"))
    {
        cur_type=GOTO;
    }
    else if(!strcmp(name,"REM"))
    {
        cur_type=REM;
    }
    else if(!strcmp(name,"END"))
    {
        cur_type=END;
    }
    else if(!strcmp(name,"IF"))
    {
        cur_type=IF;
    }
    else if(!strcmp(name,"INPUT"))
    {
        cur_type=INPUT;
    }
    else if(!strcmp(name,"INPUTS"))
    {
        cur_type=INPUTS;
    }
    else
    {
        cur_type=INVALID;
        cmd_error=E_INS_NOT;
    }

    //check if the valid line has valid syntax
    if(INVALID!=cur_type &&
            !ErrorHandler::isValidExp(cur_type,str))
        cmd_error=ErrorHandler::StmtToError(cur_type);

    //skip all the blank left
    SKIP_BLANK(str);
    Line line(cur_num,std::string(str),head_type,cur_type,cmd_error);

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
    //throw error message
    ErrorHandler::throwMsg(cmd_error);
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
    //save previous error
    QList<int> error_pre=error_highlight;
    QList<int> debug_pre=debug_highlight;
    //clear current error and debug
    error_highlight.clear();
    debug_highlight.clear();

    QString s="";
    int line_num=0;
    for(auto line:buffer)
    {
        QString tmp=QString(line.head.data())+' '+QString(line.content.data());
        s+=QString::number(line.num)+' '+tmp+'\n';
        //handle error line
        if(E_NONE!=line.get_line_error())
        {
            error_highlight.push_back(line_num);
        }
        ++line_num;
    }
    code_div->setPlainText(s);

    //unhighlight previous lines
    unhighlightAll(error_pre,debug_pre);
    //highlight all lines
    highlightAll(error_highlight,debug_highlight);
}


void Widget::openFile()
{
    vmline=0;

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
        clearProgram();
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
    else if(cmd.startsWith("INPUT")||cmd.startsWith("INPUTS"))
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
    //input args variables
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
    //clear highlight list
    error_highlight.clear();
    debug_highlight.clear();
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



