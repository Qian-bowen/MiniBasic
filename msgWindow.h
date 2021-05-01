#ifndef MSGWINDOW_H
#define MSGWINDOW_H
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>


QT_BEGIN_NAMESPACE
namespace Ui { class MsgWindow; }
QT_END_NAMESPACE


class MsgWindow:public QWidget
{
    Q_OBJECT

public:
    MsgWindow(QWidget *parent = nullptr);
    ~MsgWindow();
    void set_msg(std::string str_msg);

private:
    //message window component
    Ui::MsgWindow *ui;
    QLabel* msg_label;
    QHBoxLayout* msg_layout;

};


#endif // MSGWINDOW_H
