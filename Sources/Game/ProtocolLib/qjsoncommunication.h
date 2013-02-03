#ifndef QJSONCOMMUNICATION_H
#define QJSONCOMMUNICATION_H

#include <QObject>
#include <QVariant>
#include <QTcpSocket>
#include <QMap>
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>

#include "qjsonsocket.h"
#include "qjsonrequest.h"
#include "qjsonrequestobject.h"

class QJsonCommunication : public QObject
{
    Q_OBJECT
public:
    explicit QJsonCommunication(QTcpSocket* socket = new QTcpSocket(), QObject *parent = 0);
    virtual ~QJsonCommunication();

    void connectToHost(const QString& host, quint16 port);
    void sendRequest(const QString &request, const QVariant& optionalData,
                     const std::function<void (const QString &, const QVariant &)> callback);
    void sendNotification(const QString &request, const QVariant& optionalData = QVariant());
    void registerRequest(const QString& req, std::function<void(std::shared_ptr<QJsonRequest>)> callback);
    void registerNotification(const QString& notif, std::function<void(const QVariant&)> callback);

    void clearRequest(const QString& req);
    void clearNotification(const QString& notif);

    std::weak_ptr<QJsonCommunication> getThis() { return _this; }
    void setThis(std::weak_ptr<QJsonCommunication> val) { _this = val; }

signals:
    void socketError(QJsonCommunication& sender, QAbstractSocket::SocketError socketError);
    void connected(QJsonCommunication& sender);
    void disconnected(QJsonCommunication& sender);
    void request(QJsonCommunication& sender, std::shared_ptr<QJsonRequest> request);
    void notification(QJsonCommunication& sender, const QString& notif, const QVariant& data);

public slots:

    void onDataCame(const QVariant& data);
    void onSocketError(QAbstractSocket::SocketError socketError);
    void onConnected() { emit connected(*this); }
    void onRequestFinished(QObject* request);

private:
    std::weak_ptr<QJsonCommunication> _this;

    QJsonSocket* m_socket;

    std::mutex sendSyncRoot;

    volatile uint m_next_id;
    QMap<uint,QJsonRequestObject*> pendingRequests;
    QMap<QString, std::function<void(std::shared_ptr<QJsonRequest>)>  > requestCallbacks;
    QMap<QString, std::function<void(const QVariant&)>  > notificationCallbacks;

    void sendResponse(const QString& response, uint id, const QVariant& optionalData);

    friend class QJsonRequest;
};

#endif // QJSONCOMMUNICATION_H
