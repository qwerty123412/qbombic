#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariant>
#include <memory>
#include <QTcpSocket>
#include <functional>
#include <QModelIndex>
#include <QListView>

#include "qjsoncommunication.h"
#include "toplevel.h"
#include "qgame.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool getExitApp() { return exitApp; }

public slots:
    void setComm(std::shared_ptr<QJsonCommunication> value);
    void onSocketError(QJsonCommunication& sender,QAbstractSocket::SocketError error);
    void onChatSend();
    void logoutClick();
    void exitClick();
    void startClick();
    void refreshPlayerList();
    void gameEnded();

    void createGame();
    
private:
    Ui::MainWindow *ui;
    std::shared_ptr<QJsonCommunication> comm;
    bool exitApp;
    QString gameName;
    QGame* gameObject;
};

#endif // MAINWINDOW_H
