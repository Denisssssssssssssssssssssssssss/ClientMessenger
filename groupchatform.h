#ifndef GROUPCHATFORM_H
#define GROUPCHATFORM_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTcpSocket>
#include <QString>
#include <QLineEdit>

class GroupChatForm : public QWidget
{
    Q_OBJECT

private:
    QString login;
    QString chatId;
    QTcpSocket *socket;
    QPushButton *backButton;
    QPushButton *settingsButton;
    QListWidget *messageList;
    QLineEdit *messageEdit;
    QPushButton *sendButton;

    void appendMessageToList(const QString &message, const QString &timestamp, bool isOwnMessage); //Метод для добавления сообщений в список

private slots:
    void goBack();
    void openSettings();
    void sendMessage();
    void onReadyRead();

public:
    explicit GroupChatForm(QTcpSocket *socket, QString login, QString chatId, QWidget *parent = nullptr);
    void connectSocket();
    void loadChatHistory();

signals:
    void backRequested();
    void openSettingsRequested();
};

#endif // GROUPCHATFORM_H
