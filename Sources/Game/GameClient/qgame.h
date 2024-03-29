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
    ~QGame();
    
    void start();

    void processState(const QVariant& data);

    void moveDown();
    void moveLeft();
    void moveRight();
    void moveUp();


signals:
    void ended();
public slots:
    void processMovements(int direction);
    void bombLaid();

private:
    KAstTopLevel window;
    MainWindow* main;
    KAsteroidsView* view;
};

#endif // QGAME_H
