#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QtAlgorithms>
#include <QKeyEvent>
#include <QEventLoop>
#include <QTextBlock>
#include <QColor>
#include <QMessageBox>
#include <QRegExpValidator>


#include <iostream>
#include <algorithm>
#include <functional>
#include "program.h"
#include "error.h"
#include "msgWindow.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE



class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void buffer_out();//test function

private:
    void getCmd();
    void storeCmd(QString cur_line);
    void storeCmdWrapper(QString cur_line);
    void execCmd(StatementType type,std::string cur_line);
    void execCmdWrapper(StatementType type,std::string cur_line);


    void parseBuffer();
    void actualLineToVisual();
    void debugCode();
    CompVal parsePropmtLine(char*& prompt);
    void runErrorHandler(RunError error);

    void showProgm();
    void showPrompt();
    void showSnapshot();
    void showNextTree();
    void showMsgWindow(std::string str_msg);

    void highlightLine(int line,QColor color,QList<QTextEdit::ExtraSelection>& extras);
    void highlightLineWrapper(int line,QColor color);
    void highlightAll(QList<int>& error_hlt,QList<int>& debug_highlight);
    void unhighlightAll(QList<int>& error_hlt,QList<int>& debug_highlight);



private slots:
    void openFile();
    void getCmdWrapper();
    void runCode();
    void debugCodeWrapper();
    void clearProgram();
    void finishDebug();
    void finishRun();
    void handlePrompt();

    void handle_input(std::string var_name,StatementType type);
    void result_print(std::string result);
    void tree_print(std::string tree);

signals:
    void run_program(Mode mode);
    void run_complete();
    void debug_complete();
    void finish_input();
    void input_again();

private:
    //ui component
    Ui::Widget *ui;
    QWidget *widget;
    QVBoxLayout* layout;
    QHBoxLayout* code_result_layout,*tree_var_layout,*button_layout;
    QLineEdit *cmd_div;
    QTextBrowser *result_div,*tree_div,*code_div,*var_div;
    QPushButton* load_but,*run_but,*clear_but,*debug_but;

    //message box
    QMessageBox* msgBox;

    //ui colors
    QColor debug_green=QColor(100, 255, 100);
    QColor error_red=QColor(255, 100, 100);
    QColor uncolor_write=QColor(0,0, 255);

    //program buffer
    QList<Line> buffer;

    //actual line and visual line map,first is actual line,second is visual line
    std::unordered_map<int,int> line_map;

    //highlight list
    QList<int> error_line;
    QList<int> debug_line;

    Program program;

    std::string cmd_input_name="";
    StatementType input_type;



    int debug_next=0;//trace the line debug
    Mode cur_mode;

};
#endif // WIDGET_H
