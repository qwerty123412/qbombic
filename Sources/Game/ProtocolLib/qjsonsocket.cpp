#include "qjsonsocket.h"

#include <QDebug>

QJsonSocket::QJsonSocket(QTcpSocket* socket, QObject *parent) :
    QObject(parent), m_socket(socket), m_stream(m_socket), toRead(0)
{
    m_stream.setVersion(QDataStream::Qt_4_0);

    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(handleError(QAbstractSocket::SocketError)));
    connect(m_socket, SIGNAL(readyRead()), SLOT(readReady()));
    connect(m_socket, SIGNAL(connected()), SLOT(onConnected()));
}

QJsonSocket::~QJsonSocket()
{
    m_socket->close();

    delete(m_socket);
}

void QJsonSocket::writeData(const QVariant& variant)
{
    QJson::Serializer serializer;
    QByteArray data = serializer.serialize(variant);

    syncRoot.lock();

    //qDebug() << "Writing " << data.length() << ":" << data;
    m_stream << (decltype(toRead))data.length();
    m_stream << data;

    syncRoot.unlock();
}

void QJsonSocket::connectToHost(const QString &host, quint16 port)
{
    m_socket->connectToHost(host, port);
}

void QJsonSocket::readReady()
{
    while(true)
    {
        quint64 dataLen = m_socket->bytesAvailable();
        if (toRead == 0)
        {
            if (dataLen < (int)sizeof(toRead))
                return;

            m_stream >> toRead;
        }

        if (dataLen - (int)sizeof(toRead) < toRead)
            return;

        QJson::Parser parser;

        QByteArray json(toRead, 0);
        m_stream >> json;
        toRead = 0;

        bool ok;
        QVariant data = parser.parse(json, &ok);

        if (!ok)
        {
            qDebug() << "Cannot parse json: " << json;
            m_socket->close();
        }

        //qDebug() << json;
        emit dataCame(data);
    }
}

void QJsonSocket::handleError(QAbstractSocket::SocketError socketErr)
{
    emit socketError(socketErr);
    qDebug() << "Socket error " << socketErr;
}
