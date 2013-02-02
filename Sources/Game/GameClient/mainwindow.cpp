#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qplayerinfo.h"
#include "qgameinfo.h"
#include "RequestsResponses.h"
#include "qgame.h"

#include <QInputDialog>
#include <QStandardItemModel>
#include <qjson/qobjecthelper.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), comm(nullptr), exitApp(true)
{
    ui->setupUi(this);
    connect(ui->actionOdhlasit, SIGNAL(triggered()), SLOT(logoutClick()));
    connect(ui->actionUkoncit, SIGNAL(triggered()), SLOT(exitClick()));
    connect(ui->pushButtonSend, SIGNAL(clicked()), SLOT(onChatSend()));
    connect(ui->pushButtonRefreshPlayers, SIGNAL(clicked()), SLOT(refreshPlayerList()));
    connect(ui->pushButtonNewGame, SIGNAL(clicked()), SLOT(createGame()));
    connect(ui->pushButtonStart, SIGNAL(clicked()), SLOT(startClick()));

    ui->listViewPlayers->setModel(new QStandardItemModel(ui->listViewPlayers));
    ui->listViewGames->setModel(new QStandardItemModel(ui->listViewGames));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setComm(std::shared_ptr<QJsonCommunication> value)
{
    comm = value;
    comm-> registerNotification(Notifications::MESSAGE, [this](const QVariant& message)
    {
        QVariantMap data = message.toMap();
        this->ui->textBrowserChat->append(data["from"].toString() + ": " + data["message"].toString());
    });

    connect(comm.get(), SIGNAL(socketError(QJsonCommunication&, QAbstractSocket::SocketError)), SLOT(onSocketError(QJsonCommunication&,QAbstractSocket::SocketError)));
    refreshPlayerList();
    comm->registerNotification(Notifications::NEW_PLAYER, [this](const QVariant& message)
    {
        QPlayerInfo info;
        QJson::QObjectHelper::qvariant2qobject(message.toMap(), &info);
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->listViewPlayers->model());
        this->ui->textBrowserChat->append("![Player " + info.getName() + " has entered.]");
        model->appendRow(new QStandardItem(info.getName()));
        model->sort(0);
    });
    comm->registerNotification(Notifications::QUIT_PLAYER, [this](const QVariant& data)
    {
        QPlayerInfo info;
        QJson::QObjectHelper::qvariant2qobject(data.toMap(), &info);

        this->ui->textBrowserChat->append("![Player " + info.getName() + " has quit.]");
        refreshPlayerList();
    });
    comm->registerNotification(Notifications::GAME_LIST, [this](const QVariant& data)
    {
        QStandardItemModel* model = static_cast<QStandardItemModel*>(this->ui->listViewGames->model());
        model->clear();
        QVariantList games = data.toList();
        for (QVariant g : games)
        {
            QGameInfo info;
            QJson::QObjectHelper::qvariant2qobject(g.toMap(), &info);
            QString label = QString("%1: %2 (%3)").arg(info.getCreator()).arg(info.getName()).arg(info.getPlayers());
            model->appendRow(new QStandardItem(label));
        }
    });
    comm->registerNotification(Notifications::GAME_STARTED, [this](const QVariant& data)
    {
        this->ui->textBrowserChat->append("![Game started]");

        gameObject = new QGame(this);
        connect(gameObject, SIGNAL(ended()), SLOT(gameEnded()));
        gameObject->start();
    });
}

void MainWindow::gameEnded()
{
    delete gameObject;
    gameObject = nullptr;
}

void MainWindow::onSocketError(QJsonCommunication&, QAbstractSocket::SocketError)
{
    exitApp = false;
    close();
}

void MainWindow::onChatSend()
{
    QVariantMap data;
    data.insert("message", ui->textEditMessage->toPlainText());
    comm->sendNotification(Notifications::MESSAGE, data);
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

void MainWindow::startClick()
{
    QVariantMap data;
    data.insert("name", gameName);
    comm->sendRequest(Request::START_GAME, data, [this](const QString& resp, const QVariant& respData)
    {
        if (resp != Response::OK)
        {
            this->ui->textBrowserChat->append("![Game was not started.]");
            return;
        }

        this->ui->textBrowserChat->append("![Game was started.]");
    });
}

void MainWindow::refreshPlayerList()
{
    comm->sendRequest(Request::GET_PLAYERS, QVariant(), [this](const QString& resp, const QVariant& respData)
    {
        if (resp != Response::OK)
        {
            qDebug() << "Error (get players): " << resp;
            return;
        }

        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->listViewPlayers->model());
        model->clear();

        QVariantList plays = respData.toList();
        for(QVariant player : plays)
        {
            QPlayerInfo info;
            QJson::QObjectHelper::qvariant2qobject(player.toMap(), &info);

            model->appendRow(new QStandardItem(info.getName()));
        }
        model->sort(0);
    });
}

void MainWindow::createGame()
{
    bool ok;
    QString name;
    while(true)
    {
        name = QInputDialog::getText(
                    this,
                    "Create game",
                    "Game name:",
                    QLineEdit::Normal,
                    QString(),
                    &ok
                    );
        if (!ok)
            return;
        if (name.size())
            break;
    }
    QVariantMap data;
    data.insert("name", name);
    gameName = name;

    comm->sendRequest(Request::CREATE_GAME, data, [this](const QString& resp, const QVariant& respData)
    {
        if (resp != Response::OK)
        {
            gameName = nullptr;
            this->ui->textBrowserChat->append("![Game was not created.]");
            return;
        }

        this->ui->textBrowserChat->append("![Game was created.]");
        this->ui->pushButtonStart->setEnabled(true);
    });
}
