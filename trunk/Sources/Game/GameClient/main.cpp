#include <QApplication>
#include "mainwindow.h"
#include "logindialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    do
    {
        MainWindow mw;
        LoginDialog w(&mw);
        w.show();

        a.exec();

        if (!w.getFinishedSuccesfully())
            break;
        if (mw.getExitApp())
            break;
    }
    while(true);
}
