#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class SettingsForm : public QWidget
{
    Q_OBJECT

private:
    QTcpSocket *socket;
    QString login;
    QString nickname;
    QPushButton *backButton;
    QPushButton *blacklistButton;
    QPushButton *changeLoginButton;
    QPushButton *saveLoginButton;
    QPushButton *changeNameButton;
    QPushButton *saveNameButton;
    QPushButton *changePasswordButton;
    QLabel *loginLabel;
    QLabel *nameLabel;
    QLineEdit *loginEdit;
    QLineEdit *nameEdit;

    bool loginContainsOnlyAllowedCharacters(const QString &login);
    bool passwordContainsRequiredCharacters(const QString &password);

public:
    explicit SettingsForm(QTcpSocket *socket, QString login, QWidget *parent = nullptr);
    void connectSocket();
    void requestNickname();

signals:
    void backToMessengerFormRequested();

private slots:
    void saveName();
    void saveLogin();
    void enableNameEdit();
    void enableLoginEdit();
    void onServerResponse();
    void handleBackClick();
    void enablePasswordChange();
};

#endif // SETTINGSFORM_H
