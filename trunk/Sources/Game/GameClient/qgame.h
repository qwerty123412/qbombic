#ifndef QGAME_H
#define QGAME_H

#include <QObject>
#include "toplevel.h"

class QGame : public QObject
{
    Q_OBJECT
public:
    explicit QGame(QObject *parent = 0);
    
    void start();
signals:
    void ended();
public slots:
private:
    KAstTopLevel window;
};

#endif // QGAME_H
