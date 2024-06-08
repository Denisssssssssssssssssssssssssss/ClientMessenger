#include "settingsform.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

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
    loginEdit->setFixedWidth(200);

    changeLoginButton = new QPushButton(tr("Сменить логин"), this);
    changeLoginButton->setFixedWidth(120);
    saveLoginButton = new QPushButton(tr("Сохранить"), this);
    saveLoginButton->setFixedWidth(120);

    nameLabel = new QLabel(tr("Ваше имя:"), this);
    nameEdit = new QLineEdit(this);
    nameEdit->setReadOnly(true); // Поле для ввода, в котором нельзя писать
    nameEdit->setFixedWidth(200);

    changeNameButton = new QPushButton(tr("Сменить имя"), this);
    changeNameButton->setFixedWidth(120);
    saveNameButton = new QPushButton(tr("Сохранить"), this);
    saveNameButton->setFixedWidth(120);

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
    loginLayout->addWidget(saveLoginButton); // Добавляем кнопку сохранения рядом с кнопкой изменения
    loginLayout->addStretch();

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addStretch();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    nameLayout->addWidget(changeNameButton);
    nameLayout->addWidget(saveNameButton); // Аналогично для имени
    nameLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(loginLayout);
    mainLayout->addLayout(nameLayout);
    mainLayout->addWidget(changePasswordButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    saveLoginButton->hide();
    saveNameButton->hide();

    connect(changeLoginButton, &QPushButton::clicked, this, &SettingsForm::enableLoginEdit);
    connect(saveLoginButton, &QPushButton::clicked, this, &SettingsForm::saveLogin);
    connect(changeNameButton, &QPushButton::clicked, this, &SettingsForm::enableNameEdit);
    connect(saveNameButton, &QPushButton::clicked, this, &SettingsForm::saveName);

    setLayout(mainLayout);
}

void SettingsForm::enableLoginEdit() {
    loginEdit->setReadOnly(false); // Делаем поле ввода доступным
    changeLoginButton->hide(); // Скрываем кнопку изменения
    saveLoginButton->show(); // Показываем кнопку сохранения
}

void SettingsForm::enableNameEdit() {
    nameEdit->setReadOnly(false); // Делаем поле ввода доступным
    changeNameButton->hide(); // Скрываем кнопку изменения
    saveNameButton->show(); // Показываем кнопку сохранения
}

void SettingsForm::saveLogin() {
    QString newLogin = loginEdit->text();

    // Проверяем, что новый логин не пуст
    if(newLogin.isEmpty() && newLogin != "New user")
    {
        QMessageBox::warning(this, tr("Ошибка"), tr("Указан недопустимый логин."));
        return;
    }

    // Отправляем запрос на сервер для обновления логина
    QJsonObject request;
    request["type"] = "update_login";
    request["old_login"] = login; // Текущий логин пользователя
    request["new_login"] = newLogin; // Новый логин

    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
}

void SettingsForm::saveName() {
    // Здесь код для сохранения нового имени
    // После сохранения:
    nameEdit->setReadOnly(true); // Снова делаем поле ввода не доступным
    saveNameButton->hide(); // Скрываем кнопку сохранения
    changeNameButton->show(); // Показываем кнопку изменения
}

void SettingsForm::onServerResponse()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj.contains("status")) {
        QString status = jsonObj["status"].toString();

        if (status == "success") {
            QMessageBox::information(this, tr("Успех"), tr("Логин успешно обновлён."));
            // Обновляем текущий логин
            login = loginEdit->text();
        } else if (status == "error") {
            QString message = jsonObj.contains("message") ? jsonObj["message"].toString() :
                                  tr("Логин не соответствует правилам, либо уже используется.");
            QMessageBox::warning(this, tr("Ошибка"), message);
            // Возвращаем старое значение логина в поле, если ошибка
            loginEdit->setText(login);
        }
    }

    // После обработки ответа возвращаем интерфейс в исходное состояние
    loginEdit->setReadOnly(true);
    saveLoginButton->hide();
    changeLoginButton->show();
}

void SettingsForm::connectSocket() {
    // Connect again when RegistrationForm is shown
    connect(socket, &QTcpSocket::readyRead, this, &SettingsForm::onServerResponse);
}
