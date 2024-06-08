#ifndef MESSENGERFORM_H
#define MESSENGERFORM_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTcpSocket>
#include <QString>
#include <QJsonArray>

class MessengerForm : public QWidget
{
    Q_OBJECT

public:
    explicit MessengerForm(QTcpSocket *socket, QString login, QWidget *parent = nullptr);
    void connectSocket();

private slots:
    void openSettings();
    void logOut();
    void findUsers();
    void updateUserList(QJsonArray users);
    void onReadyRead(); // Новый слот для чтения данных из сокета

private:
    QPushButton *settingsButton;
    QPushButton *findButton;
    QLineEdit *searchEdit;
    QListWidget *chatList; //Список существующих чатов
    QListWidget *userList; //Список для найденных пользователей
    QPushButton *logOutButton;
    QTcpSocket *socket;
    QString login;

signals:
    void settingsRequested();
};

#endif // MESSENGERFORM_H
