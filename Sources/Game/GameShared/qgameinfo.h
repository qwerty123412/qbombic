#ifndef QGAMEINFO_H
#define QGAMEINFO_H


#include <QObject>
#include <qplayerinfo.h>

class QGameInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(QPlayerInfo creator READ getCreator WRITE setCreator)
    Q_PROPERTY(QList<QPlayerInfo> members READ getMembers WRITE setMembers)

public:
    explicit QGameInfo(QObject *parent = 0);

    const QString& getName() const { return name; }
    void setName(const QString& value) { name = value; }

    const QPlayerInfo& getCreator() const { return creator; }
    void setCreator(const QPlayerInfo& info) { creator = info; }

    const QList<QPlayerInfo>& getMembers() const { return members; }
    void setMembers(const QList<QPlayerInfo>& list) { members = list; }

    typename QList<QPlayerInfo>::size_type getCount() const { return members.size() + 1; }

signals:

public slots:

private:
    QString name;
    QPlayerInfo creator;
    QList<QPlayerInfo> members;

};
#endif // QGAMEINFO_H
