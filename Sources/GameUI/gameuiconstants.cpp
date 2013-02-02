#include "gameuiconstants.h"

QString GameUIConstants::mBaseDir = 0;

GameUIConstants::GameUIConstants()
{
}

void GameUIConstants::setBaseDir(const QString &dir)
 {
    mBaseDir = dir;
    qDebug() << "BaseDir: " << mBaseDir << endl;
}

const QString GameUIConstants::getBaseDir()
{
    return mBaseDir;
}
