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
//    std::map<std::string,int> mem;
//    Statement s("LET m = 25 / 5 * ( 1 + 2 ) * 3 ",mem,0);

    //return 0;
}
