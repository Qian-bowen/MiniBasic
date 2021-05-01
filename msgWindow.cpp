#include "msgWindow.h"
#include "ui_MsgWindow.h"

MsgWindow::MsgWindow(QWidget *parent)
    : QWidget(parent)
    ,ui(new Ui::MsgWindow)
{

    msg_label=new QLabel;
    msg_layout = new QHBoxLayout;

    //message window
    ui->setupUi(this);
    this->resize(400,200);
    this->setWindowTitle("Error Alarm");
    msg_layout->addWidget(msg_label);
    this->setLayout(msg_layout);
}

void MsgWindow::set_msg(std::string str_msg)
{
    msg_label->setText(QString(str_msg.data()));
}

MsgWindow::~MsgWindow()
{
    delete ui;
    delete msg_label;
    delete msg_layout;
}
