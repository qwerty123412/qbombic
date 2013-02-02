#ifndef REQUESTSRESPONSES_H
#define REQUESTSRESPONSES_H

#include <QString>
#include <QVariant>
#include <memory>
#include <qjsonrequest.h>


inline static QString extractQVariantItem(const QVariant& var, const QString& key)
{
    return var.toMap()[key].toString();
}

inline static QString extractQVariantItem(const std::shared_ptr<QJsonRequest> req, const QString& key)
{
    return extractQVariantItem(*req->getData(), key);
}

struct Response
{
    static const QString OK;
    static const QString FAILED;
};

struct Request
{
    static const QString LOGIN;//send client
    static const QString GET_PLAYERS;//send client
    static const QString CREATE_GAME;//send client
    static const QString JOIN_GAME;//send client
};

struct Notifications
{
    static const QString NEW_PLAYER;//send server
    static const QString QUIT_PLAYER;//send server
    static const QString GAME_CLOSED;//send server
    static const QString GAME_LIST;//send server
    static const QString MESSAGE;//send client (outgoing) & server (incoming)
    static const QString QUIT_GAME;//send client
};

#endif // REQUESTSRESPONSES_H
