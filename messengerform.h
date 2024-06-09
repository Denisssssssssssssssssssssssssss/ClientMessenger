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
    void requestChatList();

private slots:
    void openSettings();
    void logOut();
    void findUsers();
    void updateUserList(QJsonArray users);
    void updateChatList(QJsonArray chats);
    void onReadyRead(); //слот для чтения данных из сокета
    void onSearchTextChanged(const QString &text);
    void openChat(QListWidgetItem *item);
    void onChatListItemClicked(QListWidgetItem *item); //слот для обработки щелчка по элементу chatList

private:
    QPushButton *settingsButton;
    QLineEdit *searchEdit;
    QListWidget *chatList; //Список существующих чатов
    QListWidget *userList; //Список для найденных пользователей
    QPushButton *logOutButton;
    QTcpSocket *socket;
    QString login;

signals:
    void settingsRequested();
    void logoutRequested();
    void chatRequested(QString user);
};

#endif // MESSENGERFORM_H
