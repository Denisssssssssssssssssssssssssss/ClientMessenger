#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QTcpSocket>

class LoginForm : public QWidget
{
    Q_OBJECT

private:
    QLabel *registerLabel;
    QLabel *titleLabel;
    QLabel* openedEyeLabelPass; //Картинка открытого глаза
    QLabel* closedEyeLabelPass; //Картинка закрытого глаза
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QTcpSocket *socket;
    QString login;

    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void onRegisterClicked(); //Слот для обработки клика на ссылку "Зарегистрироваться"
    void attemptLogin();      //Слот для попытки входа в аккаунт
    void handleServerResponse();  //Добавленный слот для обработки ответа от сервера

public:
    explicit LoginForm(QTcpSocket *socket, QWidget *parent = nullptr);
    QString getLogin();
    void connectSocket();

signals:
    void registerRequested(); //Сигнал для перехода к форме регистрации
    void loginSuccess();       //Сигнал об успешном входе

};

#endif // LOGINFORM_H
