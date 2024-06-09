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
    explicit ChatForm(QTcpSocket *socket, QString login, QWidget *parent = nullptr);
    void connectSocket();
private slots:
    void goBack();
    void addToBlacklist();
    void sendMessage();

signals:
    void backRequested();

private:
    QString login;
    QTcpSocket *socket;
    QPushButton *backButton;
    QPushButton *blacklistButton;
    QListWidget *messageList;
    QLineEdit *messageEdit;
    QPushButton *sendButton;
};

#endif // CHATFORM_H
