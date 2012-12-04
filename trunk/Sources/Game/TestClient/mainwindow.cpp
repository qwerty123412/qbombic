#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), socket(nullptr)
{
    ui->setupUi(this);
    connect(ui->pushButtonConnect, SIGNAL(clicked()), SLOT(connectButtonClick()));
    connect(ui->pushButtonSend, SIGNAL(clicked()), SLOT(sendButtonClick()));
    connect(ui->pushButtonSendRequest, SIGNAL(clicked()), SLOT(requestButtonClick()));

    socket = new QJsonCommunication();
    connect(socket, SIGNAL(connected(QJsonCommunication&)), SLOT(onConnected(QJsonCommunication&)));
    connect(socket, SIGNAL(socketError(QJsonCommunication&,QAbstractSocket::SocketError)), SLOT(onSocketError(QJsonCommunication&,QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(request(QJsonCommunication&,std::shared_ptr<QJsonRequest>)), SLOT(onRequest(QJsonCommunication&,std::shared_ptr<QJsonRequest>)));
    connect(socket, SIGNAL(notification(QJsonCommunication&,QString,QVariant)), SLOT(onNotification(QJsonCommunication&,QString,QVariant)));
}

MainWindow::~MainWindow()
{
    delete ui;

    delete(socket);
}

void MainWindow::onConnected(QJsonCommunication& sender)
{
    qDebug() << "Connected";
    //QMessageBox::information(this, "Connected", "Successfully", QMessageBox::Ok);
}

void MainWindow::onSocketError(QJsonCommunication& sender,QAbstractSocket::SocketError error)
{
    qDebug() << error;
}

void MainWindow::onRequest(QJsonCommunication &sender, std::shared_ptr<QJsonRequest> request)
{
    qDebug() << "Request: " << request->getRequest() << " : " << *(request->getData());
}

void MainWindow::onNotification(QJsonCommunication &sender, const QString &notification, const QVariant &data)
{
    qDebug() << "Notification: " << notification << " : " << data;
}

void MainWindow::connectButtonClick()
{
    qDebug() << "Connecting...";
    socket->connectToHost("localhost", 9876);
}

void MainWindow::sendButtonClick()
{
    QVariantMap var;
    var.insert("test", "data");
    var.insert("other", QDateTime::currentDateTime());
    //socket->writeData(var);
    socket->sendNotification(QString("test"), QVariant(var));
}

void MainWindow::requestButtonClick()
{
    QVariantMap var;
    var.insert("other", QDateTime::currentDateTime());
    //socket->writeData(var);
    socket->sendRequest(QString("test"), QVariant(var),
                        [&](const QString& response, const QVariant& respData) -> void
    {
                        qDebug() << "Response " << response << " : " << respData;
    });
}
