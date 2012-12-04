#ifndef QJSONREQUEST_H
#define QJSONREQUEST_H

#include <QObject>
#include <QVariant>
#include <memory>

class QJsonCommunication;

class QJsonRequest : public QObject
{
    Q_OBJECT
public:
    explicit QJsonRequest(QJsonCommunication* comm, const QString& request, std::shared_ptr<QVariant> data, uint id, QObject *parent = 0);
    
    const QString& getRequest() { return request; }
    std::shared_ptr<QVariant> getData() { return data; }
    uint getId() { return id; }
    void sendResponse(const QString& response, const QVariant& optionalData = QVariant());
    QJsonCommunication* getComm() { return comm; }
signals:
    
public slots:
private:
    QString request;
    std::shared_ptr<QVariant> data;
    uint id;
    QJsonCommunication* comm;
};

#endif // QJSONREQUEST_H
