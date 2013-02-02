#include <QApplication>
#include <QtGui>

#include "mainwindow.h"
#include "logindialog.h"
#include "gameuiconstants.h"
#include "toplevel.h"

int main(int argc, char *argv[])
{
    GameUIConstants::setBaseDir(QDir::currentPath() + "/../../GameUI");

    QApplication a(argc, argv);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    KAstTopLevel topLevel;
    topLevel.setWindowTitle("Bombitch");
    topLevel.show();

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
