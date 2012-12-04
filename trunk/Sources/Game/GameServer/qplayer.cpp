#include "qplayer.h"

#include "qgameserver.h"

using namespace std::placeholders;

QPlayer::QPlayer(QGameServer* server, QJsonCommunication *comm, const QString name, QObject *parent):
    QObject(parent), server(server), comm(comm), name(name)
{
    comm->registerNotification("MESSAGE", std::bind(&QPlayer::onMessage, this, _1));
}

QPlayer::~QPlayer()
{

}

void QPlayer::onMessage(const QVariant &messageData)
{
    QVariantMap map = messageData.toMap();
    map.insert("from", name);
    server->broadcastNotification("MESSAGE", map);
}

void QPlayer::writeInfo(QPlayerInfo &info) const
{
    info.setName(name);
}
