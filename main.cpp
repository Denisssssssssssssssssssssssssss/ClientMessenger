#include "mainwindow.h"

#include <QApplication>
#include <QTcpSocket>
#include <QMessageBox>

/*!
 * \brief Главная функция приложения.
 *
 * Эта функция инициализирует приложение, устанавливает соединение с сервером
 * и создает главное окно приложения при успешном подключении.
 *
 * \param argc Количество аргументов командной строки.
 * \param argv Массив аргументов командной строки.
 * \return Код завершения приложения.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // Инициализация приложения
    QString host = "127.0.0.1";  // Адрес сервера
    quint16 port = 3000;          // Порт сервера
    QTcpSocket socket;            // Сокет для сетевого взаимодействия
    MainWindow* window = nullptr; // Указатель на главное окно

    // Подключение удалось
    QObject::connect(&socket, &QTcpSocket::connected, [&]() {
        window = new MainWindow(&socket); // Создаем окно при успешном соединении
        window->show();                   // Отображаем главное окно
    });

    // Подключение не удалось
    QObject::connect(&socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
                     [&socket](QAbstractSocket::SocketError socketError) {
                         QMessageBox::critical(nullptr, "Ошибка подключения",
                                               QString("Не удалось подключиться к серверу.\nПопробуйте позже."));
                     });

    // Подключение к серверу
    socket.connectToHost(host, port);

    return a.exec(); // Запуск основного цикла приложения
}
