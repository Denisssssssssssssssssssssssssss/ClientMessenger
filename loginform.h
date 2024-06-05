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
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QTcpSocket *socket;

private slots:
    void onRegisterClicked(); // Слот для обработки клика на ссылку "Зарегистрироваться"

public:
    explicit LoginForm(QTcpSocket *socket, QWidget *parent = nullptr);

signals:
    void registerRequested(); // Сигнал для перехода к форме регистрации

};

#endif // LOGINFORM_H
