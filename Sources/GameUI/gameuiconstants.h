#ifndef GAMEUICONSTANTS_H
#define GAMEUICONSTANTS_H

#include <QString>
#include <QDebug>

class GameUIConstants
{
private:
    static QString mBaseDir;
public:
    GameUIConstants();

    static void setBaseDir(const QString& dir);
    static const QString getBaseDir();
};

#endif // GAMEUICONSTANTS_H
