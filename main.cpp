#include <QApplication>
#include "loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginWindow w;   // ✅ CAPITAL L
    w.show();

    return a.exec();
}