#ifndef QGAME_H
#define QGAME_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include <qjson/qobjecthelper.h>

#include "toplevel.h"
#include "mainwindow.h"
#include "view.h"

class QGame : public QObject
{
    Q_OBJECT
public:
    explicit QGame(QObject *parent = 0);
    
    void start();
signals:
    void ended();
public slots:
    void onTick();
private:
    KAstTopLevel window;
    MainWindow* main;
    QTimer sendTimer;
    KAsteroidsView* view;
};

#endif // QGAME_H
