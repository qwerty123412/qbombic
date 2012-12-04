#include "qjsonrequest.h"

#include "qjsoncommunication.h"

QJsonRequest::QJsonRequest(QJsonCommunication* comm, const QString& request, std::shared_ptr<QVariant> data, uint id, QObject *parent) :
    QObject(parent), request(request), data(data), id(id), comm(comm)
{

}

void QJsonRequest::sendResponse(const QString &response, const QVariant &optionalData)
 { comm->sendResponse(response, id, optionalData); }
