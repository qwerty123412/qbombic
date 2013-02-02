#include <QCoreApplication>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "ahoj";
    std::cout << "Hello world" <<std::endl;
    return a.exec();
}
