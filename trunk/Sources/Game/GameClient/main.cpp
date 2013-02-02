#include <QApplication>
#include <QtGui>

#include "mainwindow.h"
#include "logindialog.h"
#include "gameuiconstants.h"
#include "toplevel.h"
#include "gameshared.h"

int main(int argc, char *argv[])
{
    GameUIConstants::setBaseDir(QDir::currentPath() + "/../../GameUI");
    GameShared::Init();

    QApplication a(argc, argv);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

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
