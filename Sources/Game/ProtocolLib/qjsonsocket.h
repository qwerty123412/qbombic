#ifndef QJSONSOCKET_H
#define QJSONSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <mutex>
#include <memory>
#include "qjson/parser.h"
#include "qjson/serializer.h"


class QJsonSocket : public QObject
{
    Q_OBJECT
public:
    explicit QJsonSocket(QTcpSocket* socket = new QTcpSocket(), QObject *parent = 0);
    virtual ~QJsonSocket();

    void writeData(const QVariant& variant);
    void connectToHost(const QString& host, quint16 port);
signals:
    void dataCame(const QVariant& data);
    void socketError(QAbstractSocket::SocketError socketError);
    void connected();
public slots:
    void handleError(QAbstractSocket::SocketError socketError);
    void readReady();
    void onConnected() { emit connected(); }
private:
    QTcpSocket* m_socket;
    QDataStream m_stream;
    uint toRead;
    std::mutex syncRoot;
};

#endif // QJSONSOCKET_H
