#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "protocollib.h"
#include <memory>
#include "qjsoncommunication.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    void connectButtonClick();
    void sendButtonClick();
    void requestButtonClick();

    void onConnected(QJsonCommunication&);
    void onSocketError(QJsonCommunication&,QAbstractSocket::SocketError error);

    void onRequest(QJsonCommunication& sender, std::shared_ptr<QJsonRequest> request);
    void onNotification(QJsonCommunication& sender, const QString& notification, const QVariant& data);
private:
    Ui::MainWindow *ui;
    QJsonCommunication* socket;
};

#endif // MAINWINDOW_H
