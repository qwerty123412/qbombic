#ifndef QGAMEINFO_H
#define QGAMEINFO_H


#include <QtCore/QMetaType>
#include <QObject>
#include <qplayerinfo.h>
#include <QVariantList>

class QGameInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(QString creator READ getCreator WRITE setCreator)
    Q_PROPERTY(int players READ getPlayers WRITE setPlayers)
    Q_PROPERTY(bool started READ getStarted WRITE setStarted)

public:
    explicit QGameInfo(QObject *parent = 0);
    QGameInfo(const QGameInfo& other);
    ~QGameInfo();

    const QString& getName() const { return name; }
    void setName(const QString& value) { name = value; }

    const QString& getCreator() const { return creator; }
    void setCreator(const QString& info) { creator = info; }

    int getPlayers() const { return players; }
    void setPlayers(int _players) { players = _players; }

    bool getStarted() const { return started; }
    void setStarted(bool _started) { started = _started; }

signals:

public slots:

private:
    QString name;
    QString creator;
    bool started;
    int players;
};

Q_DECLARE_METATYPE(QGameInfo)

#endif // QGAMEINFO_H

