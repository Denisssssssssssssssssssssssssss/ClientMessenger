#ifndef SETTINGSGROUPCHATFORM_H
#define SETTINGSGROUPCHATFORM_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTcpSocket>
#include <QString>
#include <QJsonArray>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QLabel>

class SettingsGroupChatForm : public QWidget
{
    Q_OBJECT

private:
    QTcpSocket *socket;
    QString nickname;
    QPushButton *backButton;

private slots:
    void handleBackClick();
    void onServerResponse();

public:
    void connectSocket();
    explicit SettingsGroupChatForm(QTcpSocket *socket, QWidget *parent = nullptr);

signals:
    void backToMessengerFormRequested();
};

#endif // SETTINGSGROUPCHATFORM_H
