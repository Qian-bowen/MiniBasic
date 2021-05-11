#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("MiniBasic");
    this->resize(1200,2000);
    layout=new QVBoxLayout(this);
    code_result_layout=new QHBoxLayout;
    tree_var_layout=new QHBoxLayout;
    button_layout=new QHBoxLayout;
    code_div=new QTextBrowser;
    cmd_div=new QLineEdit;
    result_div=new QTextBrowser;
    tree_div=new QTextBrowser;
    var_div=new QTextBrowser;
    load_but=new QPushButton("LOAD");
    run_but=new QPushButton("RUN");
    debug_but=new QPushButton("DEBUG/STEP");
    clear_but=new QPushButton("CLEAR");


    layout->addLayout(code_result_layout);
    layout->addLayout(tree_var_layout);
    layout->addLayout(button_layout);
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

    QGroupBox* tree_box=new QGroupBox(tr("语句与语法树"));
    QVBoxLayout* tree_layout=new QVBoxLayout;
    tree_layout->addWidget(tree_div);
    tree_box->setLayout(tree_layout);

    QGroupBox* var_box=new QGroupBox(tr("当前变量"));
    QVBoxLayout* var_layout=new QVBoxLayout;
    var_layout->addWidget(var_div);
    var_box->setLayout(var_layout);

    msgBox=new QMessageBox(this);

    code_result_layout->addWidget(code_box);
    code_result_layout->addWidget(result_box);
    tree_var_layout->addWidget(tree_box);
    tree_var_layout->addWidget(var_box);

    button_layout->addWidget(load_but);
    button_layout->addWidget(run_but);
    button_layout->addWidget(debug_but);
    button_layout->addWidget(clear_but);

    cur_mode=OTHER;

    //only ascii is valid in cmd line
    cmd_div->setValidator(new QRegExpValidator( QRegExp("[\\x0000-\\x007F]*")));

    connect(load_but,SIGNAL(clicked()),this,SLOT(openFile()));
    connect(run_but,SIGNAL(clicked()),this,SLOT(runCode()));
    connect(debug_but,SIGNAL(clicked()),this,SLOT(debugCodeWrapper()));
    connect(clear_but,SIGNAL(clicked()),this,SLOT(clearProgram()));
    connect(cmd_div,SIGNAL(returnPressed()),this,SLOT(getCmdWrapper()));
    connect(cmd_div,SIGNAL(returnPressed()),cmd_div,SLOT(clear()));
    connect(this,SIGNAL(run_complete()),this,SLOT(displayRltTree()));
    connect(this,SIGNAL(debug_complete()),this,SLOT(finishDebug()));
    connect(this,SIGNAL(run_complete()),this,SLOT(enableAllButton()));
    connect(this,SIGNAL(debug_complete()),this,SLOT(enableAllButton()));
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

void Widget::highlightLineWrapper(int line,QColor color)
{
    QList<QTextEdit::ExtraSelection> extras;
    highlightLine(line,color,extras);
    code_div->setExtraSelections(extras);
}

void Widget::highlightAll(QList<int>& error_hlt,QList<int>& debug_highlight)
{
    QList<QTextEdit::ExtraSelection> extras;
    for(auto& error:error_hlt)
    {
        highlightLine(error,error_red,extras);
    }
    for(auto& debug:debug_highlight)
    {
        highlightLine(debug,debug_green,extras);
    }
    code_div->setExtraSelections(extras);
}

void Widget::unhighlightAll(QList<int>& error_hlt,QList<int>& debug_highlight)
{
    QList<QTextEdit::ExtraSelection> extras;
    for(auto& error:error_hlt)
    {
        highlightLine(error,uncolor_write,extras);
    }
    for(auto& debug:debug_highlight)
    {
        highlightLine(debug,uncolor_write,extras);
    }
    code_div->setExtraSelections(extras);
}

CompVal Widget::parsePropmtLine(char*& str)
{
    CompVal res;
    if(*str=='?')
    {
        str++;
        SKIP_BLANK(str);
        if(IS_DIGIT(str))
        {
            int val;
            parse_digit(str,val);
            res=CompVal(val);
            std::cout<<"int val:"<<res.get_int_val()<<std::endl;
            args_value.push_back(res);
        }
        else if(IS_STR_DELIM(str))
        {
            char* val;
            parse_string(str,val);
            res=CompVal(std::string(val));
            args_value.push_back(res);
        }
    }
    return res;
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
    if(IS_PROMPT(str))
    {
        CompVal res=parsePropmtLine(str);
        if(cmd_input_name!="")
        {
            program.mem_add_prog(cmd_input_name,res);
            cmd_input_name="";
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
    else if(!strcmp(name,"PRINTF"))
    {
        cur_type=PRINTF;
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
    {
        cmd_error=ErrorHandler::StmtToError(cur_type);
    }

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
        result_div->append(QString(s));
        return;
    }
}

void Widget::actualLineToVisual()
{
    int visual_line=0;
    for(auto& line:buffer)
    {
        line_map.insert(std::pair<int,int>(line.num,visual_line));
        visual_line++;
    }
}

void Widget::showProgm()
{
    //save previous error
    QList<int> error_pre=error_line;
    QList<int> debug_prev=debug_line;
    //clear current error and debug
    error_line.clear();

    QString s="";
    int line_num=0;
    for(auto line:buffer)
    {
        QString tmp=QString(line.head.data())+' '+QString(line.content.data());
        s+=QString::number(line.num)+' '+tmp+'\n';
        //handle error line
        if(E_NONE!=line.get_line_error())
        {
            error_line.push_back(line_num);
        }
        ++line_num;
    }
    code_div->setPlainText(s);

    //unhighlight previous lines
    unhighlightAll(error_pre,debug_prev);
    //highlight all lines
    highlightAll(error_line,debug_line);
}

void Widget::showSnapshot()
{
    QString var_buf;
    var_buf=QString(program.prog_snapshot().data());
    var_div->clear();
    var_div->setText(var_buf);
}

void Widget::showResult()
{
    QString result_buf;
    result_buf=QString(program.get_result_buf().data());
    result_div->clear();
    result_div->setText(result_buf);
}

void Widget::showNextTree()
{
    QString tree_buf;
    tree_buf=QString(program.get_current_tree().data());
    tree_div->clear();
    tree_div->setText(tree_buf);
}

void Widget::showMsgWindow(std::string str_msg)
{
    msgBox->setText(QString(str_msg.c_str()));
    msgBox->exec();
}


void Widget::openFile()
{
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


void Widget::execCmdWrapper(StatementType type,std::string cur_line)
{
    try {
        execCmd(type,cur_line);
    }  catch (const char* s) {
        showMsgWindow(std::string(s));
    }
}

/*
 * cmd that can be executed cmd immediately after get it
*/
void Widget::execCmd(StatementType type,std::string cur_line)
{
    //check if cmd valid
    char* str;
    str_to_ptr(cur_line,str);
    if(!ErrorHandler::isValidExp(type,str))
    {
        Error cmd_error=ErrorHandler::StmtToError(type);
        ErrorHandler::throwMsg(cmd_error);
        return;
    }

    //run previous code first
    runCode();
    showProgm();

    //handle the cmd
    if(type==LET)
    {
        LetStmt cur_stmt(cur_line,program.mem_snapshot(),LET);
        cur_stmt.get_stmt_eval();
    }
    else if(type==INPUTS)
    {
        char* var;
        InputsStmt cur_stmt(cur_line,program.mem_snapshot(),INPUTS);
        cur_stmt.get_var_name(var);
        cmd_input_name=std::string(var);

        showPrompt();
        QEventLoop loop;
        connect(cmd_div,SIGNAL(returnPressed()),&loop,SLOT(quit()));
        loop.exec();
    }
    else if(type==INPUT)
    {
        char* var;
        InputStmt cur_stmt(cur_line,program.mem_snapshot(),INPUT);
        cur_stmt.get_var_name(var);
        cmd_input_name=std::string(var);

        showPrompt();
        QEventLoop loop;
        connect(cmd_div,SIGNAL(returnPressed()),&loop,SLOT(quit()));
        loop.exec();
    }
    else if(type==PRINTF)
    {
        char* var;
        PrintfStmt cur_stmt(cur_line,program.mem_snapshot(),PRINTF);
        cur_stmt.get_var_name(var);
        result_div->append(QString(var));
    }
    else if(type==PRINT)
    {
        PrintStmt cur_stmt(cur_line,program.mem_snapshot(),PRINT);
        CompVal val=cur_stmt.get_stmt_eval();

        if(val.get_type()==V_INT)
        {
            std::string s=std::to_string(val.get_int_val());
            result_div->append(QString(s.data()));
        }
        //cannot print string
        if(val.get_type()==V_STR)
        {
            ErrorHandler::throwMsg(E_UDEF_VAR);
        }
    }
}

void Widget::getCmdWrapper()
{
    try {
        getCmd();
    }  catch (const char* s) {
        std::cout<<s<<std::endl;
        showMsgWindow(std::string(s));
    }
}


void Widget::getCmd()
{
    char* name,*str;
    QString cmd_txt=cmd_div->text();

    //parse cmd name
    str_to_ptr(cmd_txt.toStdString(),str);
    SKIP_BLANK(str);

    //if is program buffer or prompt
    if(IS_DIGIT(str)||IS_PROMPT(str))
    {
        storeCmdWrapper(cmd_txt);
        showProgm();
        return;
    }

    //if  is cmd to be immediately executed
    parse_symbol(str,name);
    std::string cmd=std::string(name);
    std::string line_remain(str);
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
    else if(cmd=="LET")
    {
        execCmdWrapper(LET,line_remain);
    }
    else if(cmd=="INPUTS")
    {
        execCmdWrapper(INPUTS,line_remain);
    }
    else if(cmd=="INPUT")
    {
        execCmdWrapper(INPUT,line_remain);
    }
    else if(cmd=="PRINTF")
    {
        execCmdWrapper(PRINTF,line_remain);
    }
    else if(cmd=="PRINT")
    {
        execCmdWrapper(PRINT,line_remain);
    }
    else
    {
        ErrorHandler::throwMsg(E_INV_CMD);
    }
}

void Widget::showPrompt()
{
    cmd_div->clear();
    cmd_div->insert(QString("? "));
}

void Widget::runCode()
{
    //check if program is empty
    if(buffer.empty()) return;

    //check whether program has syntax error
    if(!error_line.empty())
    {
        showMsgWindow("YOU HAVE TO FIX ALL SYNTAX ERROR FIRST");
        return;
    }

    //do line mapping
    actualLineToVisual();

    //input args variables
    int args_num;
    try {
        args_num=program.load_into_prog(buffer);
    }  catch (char const* s) {
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
    catch (RunError error)
    { 
        runErrorHandler(error);
        return;
    }

    emit run_complete();
}

void Widget::debugCodeWrapper()
{
    try {
        debugCode();
    }  catch (RunError error) {
        runErrorHandler(error);
        return;
    }
}

void Widget::runErrorHandler(RunError error)
{
    int error_line_num=-1;
    int error_pc=error.get_error_pc();
    const char* msg=error.get_error_msg();

    //find error line and highlight it
    auto iter=line_map.find(error_pc);
    if(iter!=line_map.end())
    {
        error_line_num=iter->second;
        error_line.push_back(error_line_num);
        highlightAll(error_line,debug_line);

    }

    //show error msg
    showMsgWindow(msg);
    program.clear_program();
}

/*
 * execute program line by line
 * highlight line is the line to be executed
*/
void Widget::debugCode()
{
    //check whether program has syntax error
    if(!error_line.empty())
    {
        showMsgWindow("YOU HAVE TO FIX ALL SYNTAX ERROR FIRST");
        return;
    }

    if(buffer.empty())
    {
        return;
    }

    //first enter debug mode
    if(cur_mode==OTHER)
    {
        //first check everything
        actualLineToVisual();
        program.clear_program();

        //set mode
        cur_mode=DEBUG;
        load_but->setEnabled(false);
        clear_but->setEnabled(false);
        debug_next=0;

        //load program
        int args_num;
        try {
            args_num=program.load_into_prog(buffer);
        }  catch (char const* s) {
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

        program.load_args_value(args_value);


        //unhighlight
        unhighlightAll(error_line,debug_line);
        if(!debug_line.empty()) debug_line.pop_back();
        //push next
        debug_line.push_back(debug_next);
        //highlight
        highlightAll(error_line,debug_line);
        //show tree
        showNextTree();
        return;
    }

    //run current statement
    bool is_end=program.run_one_step();
    showSnapshot();
    showResult();

    //if nothing left,return
    if(!is_end)
    {
        emit debug_complete();
        return;
    }

    //show next tree
    showNextTree();
    int actual_line=program.get_pc();
    debug_next=line_map.find(actual_line)->second;
    //unhighlight
    unhighlightAll(error_line,debug_line);
    if(!debug_line.empty()) debug_line.pop_back();
    debug_line.push_back(debug_next);
    //highlight
    highlightAll(error_line,debug_line);

}

void Widget::finishDebug()
{
    cur_mode=OTHER;
    showMsgWindow("finish debug");
}

void Widget::enableAllButton()
{
    load_but->setEnabled(true);
    clear_but->setEnabled(true);
    debug_but->setEnabled(true);
    run_but->setEnabled(true);
}

/*
* clear buffer and program
*/
void Widget::clearProgram()
{
    //clear program
    program.clear_program();
    //clear div
    result_div->clear();
    tree_div->clear();
    code_div->clear();
    cmd_div->clear();
    var_div->clear();
    //todo clear buffer
    buffer.clear();
    //clear line map
    line_map.clear();
    //clear all argvs
    args_value.clear();
    //clear highlight list
    error_line.clear();
    debug_line.clear();
    //reset debug trace
    debug_next=0;
    //set mode
    cur_mode=OTHER;
    //enable buttons
    enableAllButton();

}

void Widget::displayRltTree()
{
    QString result_buf,tree_buf,var_buf;

    result_buf=QString(program.get_result_buf().data());
    tree_buf=QString(program.get_tree_buf().data());
    var_buf=QString(program.prog_snapshot().data());
    result_div->clear();
    tree_div->clear();
    var_div->clear();
    result_div->setText(result_buf);
    tree_div->setText(tree_buf);
    var_div->setText(var_buf);
}

Widget::~Widget()
{
    delete ui;
    delete layout;
    delete code_result_layout;
    delete tree_var_layout;
    delete button_layout;
    delete cmd_div;
    delete result_div;
    delete tree_div;
    delete code_div;
    delete var_div;
    delete load_but;
    delete run_but;
    delete clear_but;
    delete debug_but;
    delete widget;
}

void Widget::buffer_out()
{
    for(auto it=buffer.begin();it!=buffer.end();++it)
    {
        std::cout<<(*it).content<<std::endl;
    }
}



