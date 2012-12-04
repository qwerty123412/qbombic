#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButtonSend, SIGNAL(clicked()), SLOT(onSend()));

    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), SLOT(onNewConnected()));
    if (server->listen(QHostAddress::Any, 9876))
        qDebug() << "Listening....";
}

MainWindow::~MainWindow()
{
    delete ui;
    delete(server);

    for(auto client : clients)
    {
        delete(client);
    }
}


void MainWindow::onRequest(QJsonCommunication &sender, std::shared_ptr<QJsonRequest> request)
{
    qDebug() << "Request: " << request->getRequest() << " : " << *(request->getData());

    QVariantMap var;
    var.insert("other", QDateTime::currentDateTime());
    request->sendResponse("OK", var);
}

void MainWindow::onNotification(QJsonCommunication &sender, const QString &notification, const QVariant &data)
{
    qDebug() << "Notification: " << notification << " : " << data;
}

void MainWindow::onNewConnected()
{
    QTcpSocket* socket = server->nextPendingConnection();
    qDebug() << "New connection...";
    QJsonCommunication* jsocket = new QJsonCommunication(socket);
    clients.push_back(jsocket);
    connect(jsocket, SIGNAL(request(QJsonCommunication&,std::shared_ptr<QJsonRequest>)), SLOT(onRequest(QJsonCommunication&,std::shared_ptr<QJsonRequest>)));
    connect(jsocket, SIGNAL(notification(QJsonCommunication&,QString,QVariant)), SLOT(onNotification(QJsonCommunication&,QString,QVariant)));

    server->close();
}

void MainWindow::onSend()
{
    for(auto client : clients)
    {
        QVariantMap var;
        var.insert("test", "data");
        var.insert("other", QDateTime::currentDateTime());
        client->sendNotification(QString("test"), QVariant(var));
    }
}
