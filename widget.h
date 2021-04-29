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


#include <iostream>
#include <algorithm>
#include "program.h"
#include "error.h"


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
    void storeCmd(QString cur_line);
    void storeCmdWrapper(QString cur_line);//with handle fault
    void parseBuffer();

    void showProgm();
    void showPrompt();

    void highlightLine(int line,QColor color,QList<QTextEdit::ExtraSelection>& extras);
    void highlightAll(QList<int>& error_hlt,QList<int>& debug_hlt);
    void unhighlightAll(QList<int>& error_hlt,QList<int>& debug_hlt);

private slots:
    void openFile();
    void getCmd();
    void runCode();
    void clearProgram();
    void displayRltTree();

signals:
    void run_complete();;


private:
    //ui component
    Ui::Widget *ui;
    QWidget *widget;
    QVBoxLayout* layout;
    QHBoxLayout* code_result_layout,*button_layout;
    QLineEdit *cmd_div;
    QTextBrowser *result_div,*tree_div,*code_div;
    QPushButton* load_but,*run_but,*clear_but;

    //program buffer
    QList<Line> buffer;

    //highlight list
    QList<int> error_highlight;
    QList<int> debug_highlight;

    QString result_buf,tree_buf;
    Program program;
    std::list<CompVal> args_value;
    int vmline=0;//line without number

};
#endif // WIDGET_H
