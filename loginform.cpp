#include "LoginForm.h"

LoginForm::LoginForm(QTcpSocket *socket, QWidget *parent) : socket(socket), QWidget(parent)
{
    registerLabel = new QLabel(tr("Еще нет аккаунта? <a href=\"register\" style=\"color:#1E90FF;\">Зарегистрироваться</a>"));
    QFont titleFont = registerLabel->font();
    titleFont.setPointSize(10); //Размер шрифта
    registerLabel->setFont(titleFont);
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(registerLabel, 0, Qt::AlignTop | Qt::AlignRight);
    titleLabel = new QLabel(tr("Вход"));
    titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleLabel->setFont(titleFont);
    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText(tr("Введите логин..."));
    loginEdit->setFixedWidth(200);
    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText(tr("Введите пароль..."));
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedWidth(200);
    loginButton = new QPushButton(tr("Войти"));
    loginButton->setFixedWidth(120);
    QVBoxLayout *centerLayout = new QVBoxLayout();
    centerLayout->addStretch(1); //Начало макета с растяжимым пространством
    centerLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(loginEdit, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(passwordEdit, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(loginButton, 0, Qt::AlignCenter);
    centerLayout->addStretch(1); //Конец макета с растяжимым пространством
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->setStretchFactor(topLayout, 1); //Меньший вес для topLayout
    mainLayout->addLayout(centerLayout);
    mainLayout->setStretchFactor(centerLayout, 9); //Больший вес для centerLayout
    setLayout(mainLayout);

    connect(registerLabel, &QLabel::linkActivated, this, &LoginForm::onRegisterClicked);

}

void LoginForm::onRegisterClicked()
{
    loginEdit->clear();
    passwordEdit->clear();
    emit registerRequested();
}
