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
    void onReadyRead();
    void onSearchTextChanged(const QString &text);
    void openChat(QListWidgetItem *item);
    void onChatListItemClicked(QListWidgetItem *item);

private:
    QPushButton *settingsButton;
    QLineEdit *searchEdit;
    QListWidget *chatList;
    QListWidget *userList;
    QPushButton *logOutButton;
    QTcpSocket *socket;
    QString login;

signals:
    void settingsRequested();
    void logoutRequested();
    void chatRequested(QString chatId, QString userNickname); // Объявление сигнала chatRequested
    void chatIdReceived(QString chatId);
};

#endif // MESSENGERFORM_H
