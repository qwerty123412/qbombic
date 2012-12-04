#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "qjsoncommunication.h"
#include "mainwindow.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LoginDialog(MainWindow* window, QWidget *parent = 0);
    ~LoginDialog();

    bool getFinishedSuccesfully() { return finishedSuccesfully; }

public slots:
    void onConnected(QJsonCommunication& sender);
    void onLoginButton();
    void onSocketError(QJsonCommunication&,QAbstractSocket::SocketError);

private:

    void newComm();
    void loginContinue();
    void destroyComm();

    Ui::LoginDialog *ui;
    std::shared_ptr<QJsonCommunication> comm;
    MainWindow* mainWindow;
    bool finishedSuccesfully;
};

#endif // LOGINDIALOG_H
