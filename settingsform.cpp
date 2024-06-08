#include "settingsform.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

SettingsForm::SettingsForm(QTcpSocket *socket, QString login, QWidget *parent) : socket(socket), login(login)
{
    backButton = new QPushButton(tr("Назад"), this);
    backButton->setFixedWidth(120);

    blacklistButton = new QPushButton(tr("Чёрный список"), this);
    blacklistButton->setFixedWidth(120);

    loginLabel = new QLabel(tr("Ваш логин:"), this);
    loginEdit = new QLineEdit(this);
    loginEdit->setText(login);
    loginEdit->setReadOnly(true); // Поле для ввода, в котором нельзя писать

    changeLoginButton = new QPushButton(tr("Сменить логин"), this);

    nameLabel = new QLabel(tr("Ваше имя:"), this);
    nameEdit = new QLineEdit(this);
    nameEdit->setReadOnly(true); // Поле для ввода, в котором нельзя писать

    changeNameButton = new QPushButton(tr("Сменить имя"), this);

    changePasswordButton = new QPushButton(tr("Сменить пароль"), this);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(blacklistButton);

    QHBoxLayout *loginLayout = new QHBoxLayout();
    loginLayout->addStretch();
    loginLayout->addWidget(loginLabel);
    loginLayout->addWidget(loginEdit);
    loginLayout->addWidget(changeLoginButton);
    loginLayout->addStretch();

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addStretch();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    nameLayout->addWidget(changeNameButton);
    nameLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(loginLayout);
    mainLayout->addLayout(nameLayout);
    mainLayout->addWidget(changePasswordButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    setLayout(mainLayout);
}
