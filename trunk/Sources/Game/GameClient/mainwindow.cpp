#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qplayerinfo.h"
#include "RequestsResponses.h"

#include <QStandardItemModel>
#include <qjson/qobjecthelper.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), comm(nullptr), exitApp(true)
{
    ui->setupUi(this);
    connect(ui->pushButtonSend, SIGNAL(clicked()), SLOT(onChatSend()));
    connect(ui->actionOdhlasit, SIGNAL(triggered()), SLOT(logoutClick()));
    connect(ui->actionUkoncit, SIGNAL(triggered()), SLOT(exitClick()));
    connect(ui->pushButtonRefreshPlayers, SIGNAL(clicked()), SLOT(onPlayersClicked()));

    ui->listViewPlayers->setModel(new QStandardItemModel(ui->listViewPlayers));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setComm(std::shared_ptr<QJsonCommunication> value)
{
    comm = value;
    comm-> registerNotification(REQ_MESSAGE, [this](const QVariant& message)
    {
        QVariantMap data = message.toMap();
            this->ui->textBrowserChat->append(data["from"].toString() + ": " + data["message"].toString());
    });

    connect(comm.get(), SIGNAL(socketError(QJsonCommunication&, QAbstractSocket::SocketError)), SLOT(onSocketError(QJsonCommunication&,QAbstractSocket::SocketError)));
}

void MainWindow::onSocketError(QJsonCommunication& sender,QAbstractSocket::SocketError error)
{
    exitApp = false;
    close();
}

void MainWindow::onChatSend()
{
    QVariantMap data;
    data.insert("message", ui->textEditMessage->toPlainText());
    comm->sendNotification(REQ_MESSAGE, data);
    ui->textEditMessage->setPlainText("");
}

void MainWindow::logoutClick()
{
    exitApp = false;
    close();
}

void MainWindow::exitClick()
{
    exitApp = true;
    close();
}

void MainWindow::onPlayersClicked()
{
    comm->sendRequest(REQ_GET_PLAYERS, QVariant(), [this](const QString& resp, const QVariant& respData)
    {
        if (resp != RESP_OK)
        {
            qDebug() << "Error (get players): " << resp;
            return;
        }

        QStandardItemModel* model = (QStandardItemModel*)ui->listViewPlayers->model();
        model->clear();

        QVariantList plays = respData.toList();
        for(QVariant player : plays)
        {
            QPlayerInfo info;
            QJson::QObjectHelper::qvariant2qobject(player.toMap(), &info);

            model->appendRow(new QStandardItem(info.getName()));
        }
    });
}
