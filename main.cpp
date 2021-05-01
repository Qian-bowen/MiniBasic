#include "widget.h"

#include <QApplication>
#include "parser.h"
#include "statement.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
