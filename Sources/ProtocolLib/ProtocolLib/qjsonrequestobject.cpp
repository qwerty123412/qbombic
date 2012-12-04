#include "qjsonrequestobject.h"

QJsonRequestObject::QJsonRequestObject(uint id, std::function<void (const QString &, const QVariant &)> callback, QObject *parent):
    QObject(parent), id(id), callback(callback)
{
}
