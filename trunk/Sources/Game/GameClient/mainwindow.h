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

class QGame;

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

    std::shared_ptr<QJsonCommunication> getComm() const { return comm; }

public slots:
    void setComm(std::shared_ptr<QJsonCommunication> value);
    void onSocketError(QJsonCommunication& sender,QAbstractSocket::SocketError error);
    void onChatSend();
    void logoutClick();
    void exitClick();
    void startClick();
    void refreshPlayerList();
    void refreshGameList(const QString&, const QVariant& data);
    void gameEnded();

    void createGame();
    void leaveGame();

    void gameSelected(QModelIndex index);

    void setPlayerName(const QString& name) { playerName = name; }
    const QString& getPlayerName() const { return playerName; }

    const QString& getGameName() const { return gameName; }


private:
    Ui::MainWindow *ui;
    std::shared_ptr<QJsonCommunication> comm;
    bool exitApp;
    QString gameName;
    QString playerName;
    QGame* gameObject;
};

#endif // MAINWINDOW_H
