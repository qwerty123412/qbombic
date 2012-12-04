#include "logindialog.h"
#include "ui_logindialog.h"

#include "RequestsResponses.h"

LoginDialog::LoginDialog(MainWindow* window, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog), comm(nullptr) , mainWindow(window), finishedSuccesfully(false)
{
    ui->setupUi(this);
    connect(ui->pushButtonConnect, SIGNAL(clicked()), SLOT(onLoginButton()));

    newComm();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::onConnected(QJsonCommunication &sender)
{
    ui->textBrowserLog->append("Connected");

    QVariantMap data;
    data.insert("name", ui->lineEditName->text());
    comm->sendRequest(REQ_LOGIN, data, [this](const QString& resp, const QVariant& respData)
    {
        if (resp != RESP_OK)
        {
             this->ui->pushButtonConnect->setEnabled(true);
             this->ui->textBrowserLog->append("Login error: " + resp);

             destroyComm();
             newComm();
             return;
        }

        qDebug() << "Login OK";
        this->ui->textBrowserLog->append("Login OK");
        loginContinue();
    });
}

void LoginDialog::onLoginButton()
{
    ui->pushButtonConnect->setEnabled(false);
    ui->textBrowserLog->setText("");
    ui->textBrowserLog->append("Connecting");
    comm->connectToHost(ui->lineEditHost->text(), ui->spinBoxPort->value());
}

void LoginDialog::onSocketError(QJsonCommunication & sender, QAbstractSocket::SocketError error)
{
    QString output;
    QDebug(&output) << error;

    ui->textBrowserLog->append(output);
    ui->pushButtonConnect->setEnabled(true);
}

void LoginDialog::loginContinue()
{
    mainWindow->setComm(comm);
    destroyComm();
    finishedSuccesfully = true;

    close();
    mainWindow->show();
}

void LoginDialog::newComm()
{
    comm = std::shared_ptr<QJsonCommunication>(new QJsonCommunication());
    connect(comm.get(), SIGNAL(connected(QJsonCommunication&)), SLOT(onConnected(QJsonCommunication&)));
    connect(comm.get(), SIGNAL(socketError(QJsonCommunication&,QAbstractSocket::SocketError)), SLOT(onSocketError(QJsonCommunication&,QAbstractSocket::SocketError)));
}

void LoginDialog::destroyComm()
{
    disconnect(comm.get(), SIGNAL(connected(QJsonCommunication&)));
    disconnect(comm.get(), SIGNAL(socketError(QJsonCommunication&,QAbstractSocket::SocketError)));
    comm = nullptr;
}
