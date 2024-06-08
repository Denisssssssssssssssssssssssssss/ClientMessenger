#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "loginform.h"
#include "registrationform.h"
#include "nicknameform.h"
#include "messengerform.h"
#include "settingsform.h"

#include <QMainWindow>
#include <QJsonObject>
#include <QString>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    LoginForm *loginForm;
    RegistrationForm *registrationForm;
    NicknameForm *nicknameForm;
    MessengerForm *messengerForm;
    SettingsForm *settingsForm;
    unsigned int window_width = 500;
    unsigned int window_height = 500;
    QTcpSocket *socket;
    QString login;

    void isNicknameNewUser();

private slots:
    void showLoginForm();
    void showRegistrationForm();
    void showMessengerForm();
    void showNicknameForm();
    void onLoginSuccess();
    void receiveNicknameStatus(); // Слот без параметров
    void showSettingsForm();

protected:
    void closeEvent(QCloseEvent *event) override;

public:
    MainWindow(QTcpSocket* socket, QWidget *parent = nullptr);

signals:
    void loginRequested();  // Сигнал для переключения на форму входа
    void checkNicknameStatus(const QString& login);  // Сигнал для проверки статуса никнейма

};
#endif // MAINWINDOW_H
