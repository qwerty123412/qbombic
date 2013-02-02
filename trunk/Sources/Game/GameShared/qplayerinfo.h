#ifndef QPLAYERINFO_H
#define QPLAYERINFO_H

#include <QObject>

class QPlayerInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)

public:
    explicit QPlayerInfo(QObject *parent = 0);
    QPlayerInfo(const QPlayerInfo& another);
    
    const QString& getName() const { return name; }
    void setName(const QString& value) { name = value; }

    QPlayerInfo& operator=(const QPlayerInfo& another)
    {
        if(&another != this)
            name = another.name;
        return *this;
    }

signals:
    
public slots:

private:
    QString name;

    
};

#endif // QPLAYERINFO_H
