#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QList>

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
    void onNewConnected();
    void onSend();
    void onRequest(QJsonCommunication &sender, std::shared_ptr<QJsonRequest> request);
    void onNotification(QJsonCommunication &sender, const QString &notification, const QVariant &data);
    
private:
    Ui::MainWindow *ui;
    QTcpServer* server;
    QList<QJsonCommunication*> clients;
};

#endif // MAINWINDOW_H
