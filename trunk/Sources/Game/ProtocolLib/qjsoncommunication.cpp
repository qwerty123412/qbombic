#include "qjsoncommunication.h"


QJsonCommunication::QJsonCommunication(QTcpSocket* socket, QObject *parent) :
    QObject(parent), m_socket(new QJsonSocket(socket)), m_next_id(0)
{
    connect(m_socket, SIGNAL(connected()), SLOT(onConnected()));
    connect(m_socket, SIGNAL(dataCame(QVariant)), SLOT(onDataCame(QVariant)));
    connect(m_socket, SIGNAL(socketError(QAbstractSocket::SocketError)), SLOT(onSocketError(QAbstractSocket::SocketError)));
}

QJsonCommunication::~QJsonCommunication()
{
    delete(m_socket);
}

void QJsonCommunication::connectToHost(const QString &host, quint16 port)
{
    m_socket->connectToHost(host, port);
}

void QJsonCommunication::sendRequest(const QString &request, const QVariant& optionalData,
                                     const std::function<void (const QString &, const QVariant &)> callback)
{
    sendSyncRoot.lock();

    uint id;
    do
    {
        id = ++m_next_id;
    }
    while(pendingRequests.contains(id));

    QJsonRequestObject* req = new QJsonRequestObject(id, callback);
    pendingRequests.insert(id, req);

    sendSyncRoot.unlock();

    QVariantMap reqData;
    reqData.insert("request", request);
    reqData.insert("id", id);
    reqData.insert("data", optionalData);
    m_socket->writeData(reqData);
}

void QJsonCommunication::sendNotification(const QString &request, const QVariant& optionalData)
{
    QVariantMap req;
    req.insert("notification", request);
    req.insert("data", optionalData);
    m_socket->writeData(req);
}

void QJsonCommunication::sendResponse(const QString& response, uint id, const QVariant &optionalData)
{
    QVariantMap req;
    req.insert("response", response);
    req.insert("id", id);
    req.insert("data", optionalData);
    m_socket->writeData(req);
}

void QJsonCommunication::onSocketError(QAbstractSocket::SocketError socketErr)
{
    emit socketError(*this, socketErr);
    emit disconnected(*this);
}

void QJsonCommunication::onDataCame(const QVariant& data)
{
    QVariantMap map = data.toMap();

    std::shared_ptr<QVariant> dataOpt = map.contains("data") ? std::shared_ptr<QVariant>( new QVariant( map["data"] ) ) : nullptr;

    if (map.contains("notification"))
    {
        QString notifString = map["notification"].toString();
        if (notificationCallbacks.contains(notifString))
            notificationCallbacks[notifString](*dataOpt);
        else
            emit notification(*this, notifString, *dataOpt);
    }
    else if (map.contains("request"))
    {
        QString reqString = map["request"].toString();
        uint id = map["id"].toUInt();
        std::shared_ptr<QJsonRequest> req = std::shared_ptr<QJsonRequest>(
                    new QJsonRequest(
                        this,
                        reqString,
                        dataOpt,
                        id
                               )
                    );

        connect(&*req, SIGNAL(destroyed(QObject*)), SLOT(onRequestFinished(QObject*)));

        if (requestCallbacks.contains(reqString))
            requestCallbacks[reqString](req);
        else
            emit request(*this, req);
    }
    else if (map.contains("response"))
    {
        uint id = map["id"].toUInt();

        sendSyncRoot.lock();

        QJsonRequestObject* req = pendingRequests[id];

        sendSyncRoot.unlock();

        if (req == nullptr)
        {
            qDebug() << "Unknown id: " << id;
            return;
        }

        req->response(map["response"].toString(), *dataOpt);
    }
    else if (map.contains("finished"))
    {        
        uint id = map["finished"].toUInt();

        sendSyncRoot.lock();

        QJsonRequestObject* req = pendingRequests[id];
        pendingRequests.remove(id);

        sendSyncRoot.unlock();

        if (req != nullptr)
            delete(req);
    }
    else
        qDebug() << "Unknown data: " << data;
}

void QJsonCommunication::onRequestFinished(QObject *request)
{
    QVariantMap req;
    req.insert("finished", ((QJsonRequest*)request)->getId());
    m_socket->writeData(req);
}

void QJsonCommunication::registerRequest(const QString &req, std::function<void (std::shared_ptr<QJsonRequest>)> callback)
{
    requestCallbacks.insert(req, callback);
}

void QJsonCommunication::registerNotification(const QString &req, std::function<void(const QVariant &)> callback)
{
    notificationCallbacks.insert(req, callback);
}
