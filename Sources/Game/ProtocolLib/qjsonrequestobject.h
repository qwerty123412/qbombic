#ifndef QJSONREQUESTOBJECT_H
#define QJSONREQUESTOBJECT_H

#include <QObject>
#include <functional>

class QJsonRequestObject : public QObject
{
    Q_OBJECT
public:
    explicit QJsonRequestObject(uint id, std::function<void (const QString &, const QVariant &)> callback, QObject *parent = 0);
    uint getId() { return id; }
    void response(const QString& response, const QVariant& data) { callback(response, data); }
signals:
    
public slots:

private:
    uint id;
    std::function<void (const QString &, const QVariant &)> callback;
};

#endif // QJSONREQUESTOBJECT_H
