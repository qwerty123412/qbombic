#ifndef QPLAYERINFO_H
#define QPLAYERINFO_H

#include <QObject>

class QPlayerInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ getName WRITE setName)

public:
    explicit QPlayerInfo(QObject *parent = 0);
    
    QString getName() const { return name; }
    void setName(const QString& value) { name = value; }

signals:
    
public slots:

private:
    QString name;

    
};

#endif // QPLAYERINFO_H
