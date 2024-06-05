#include "mainwindow.h"

#include <QApplication>
#include <QTcpSocket>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString host = "127.0.0.1";
    quint16 port = 3000;
    QTcpSocket socket;
    MainWindow* window = nullptr;

    //Подключение удалось
    QObject::connect(&socket, &QTcpSocket::connected, [&]() {
        window = new MainWindow(&socket); // Создаем окно при успешном соединении
        window->show();
    });

    //Подключение не удалось
    QObject::connect(&socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
        [&socket](QAbstractSocket::SocketError socketError) {
            QMessageBox::critical(nullptr, "Ошибка подключения", QString("Не удалось подключиться к серверу.\nПопробейте позже."));
    });

    socket.connectToHost(host, port);
    return a.exec();
}
