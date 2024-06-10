#ifndef CHATFORM_H
#define CHATFORM_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTcpSocket>
#include <QString>
#include <QLineEdit>

class ChatForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChatForm(QTcpSocket *socket, QString login, QString chatId, QWidget *parent = nullptr);
    void connectSocket();
    void loadChatHistory();

private slots:
    void goBack();
    void addToBlacklist();
    void sendMessage();
    void onReadyRead();

signals:
    void backRequested();

private:
    QString login;
    QString chatId;
    QTcpSocket *socket;
    QPushButton *backButton;
    QPushButton *blacklistButton;
    QListWidget *messageList;
    QLineEdit *messageEdit;
    QPushButton *sendButton;

    void appendMessageToList(const QString &message, const QString &timestamp, bool isOwnMessage); //метод для добавления сообщений в список
};

#endif // CHATFORM_H
