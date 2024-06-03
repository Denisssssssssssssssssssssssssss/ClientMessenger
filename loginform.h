#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);

signals:
    void registerRequested(); // Сигнал для перехода к форме регистрации

private slots:
    void onRegisterClicked(); // Слот для обработки клика на ссылку "Зарегистрироваться"

private:
    QLabel *registerLabel;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
};

#endif // LOGINFORM_H
