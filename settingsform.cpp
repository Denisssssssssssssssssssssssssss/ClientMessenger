#include "settingsform.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QInputDialog>
#include <QCryptographicHash>

SettingsForm::SettingsForm(QTcpSocket *socket, QString login, QWidget *parent) : socket(socket), login(login)
{
    backButton = new QPushButton(tr("Назад"), this);
    backButton->setFixedWidth(120);

    blacklistButton = new QPushButton(tr("Чёрный список"), this);
    blacklistButton->setFixedWidth(120);

    loginLabel = new QLabel(tr("Ваш логин:"), this);
    loginEdit = new QLineEdit(this);
    loginEdit->setText(login);
    loginEdit->setReadOnly(true); //Поле для ввода, в котором нельзя писать
    loginEdit->setFixedWidth(200);

    changeLoginButton = new QPushButton(tr("Сменить логин"), this);
    changeLoginButton->setFixedWidth(120);
    saveLoginButton = new QPushButton(tr("Сохранить"), this);
    saveLoginButton->setFixedWidth(120);

    nameLabel = new QLabel(tr("Ваше имя:"), this);
    nameEdit = new QLineEdit(this);
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
    loginLayout->addWidget(saveLoginButton);
    loginLayout->addStretch();

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addStretch();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    nameLayout->addWidget(changeNameButton);
    nameLayout->addWidget(saveNameButton);
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
    connect(backButton, &QPushButton::clicked, this, &SettingsForm::handleBackClick);
    connect(changePasswordButton, &QPushButton::clicked, this, &SettingsForm::enablePasswordChange);
    nameEdit->setText(nickname);
    nameEdit->setReadOnly(true); //Поле для ввода, в котором нельзя писать

    setLayout(mainLayout);
}

//Разрешить редактирование поля для ввода логина
void SettingsForm::enableLoginEdit()
{
    loginEdit->setReadOnly(false);
    changeLoginButton->hide();
    saveLoginButton->show();
}

//Разрешить редактирование поля для ввода имени (nickname)
void SettingsForm::enableNameEdit()
{
    nameEdit->setReadOnly(false);
    changeNameButton->hide();
    saveNameButton->show();
}

//Сохранение введенного логина
void SettingsForm::saveLogin()
{
    QString newLogin = loginEdit->text();

    if(newLogin == login) {
        saveLoginButton->hide();
        changeLoginButton->show();
        loginEdit->setReadOnly(true);
        QMessageBox::information(this, tr("Информация"), tr("Логин не был изменён."));
        return;
    }

    //Проверяем, что новый логин не пуст и содержит только допустимые символы
    if(newLogin.isEmpty() || !loginContainsOnlyAllowedCharacters(newLogin)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Указан недопустимый логин."));
        return;
    }

    //Запрос пароля у пользователя
    bool ok;
    QString password = QInputDialog::getText(this, tr("Подтверждение логина"),
        tr("Введите ваш пароль:"), QLineEdit::Password, "", &ok);

    //Если пользователь нажал OK и ввёл пароль
    if(ok && !password.isEmpty())
    {
        QJsonObject request;
        request["type"] = "update_login";
        request["old_login"] = login;
        request["new_login"] = newLogin;
        request["password"] = password;

        QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
        socket->write(requestData);
        socket->flush();
    }
    else if (ok)
    {
        QMessageBox::warning(this, tr("Ошибка"), tr("Пароль не может быть пустым."));
    }
}

//Сохранение введенного имени (nickname)
void SettingsForm::saveName()
{
    QString newName = nameEdit->text();

    //Проверяем, что новое имя не пустое и не по умолчанию
    if(newName.isEmpty() || newName == "New user")
    {
        QMessageBox::warning(this, tr("Ошибка"), tr("Указано недопустимое имя."));
        return;
    }

    QJsonObject request;
    request["type"] = "update_nickname";
    request["login"] = login;
    request["nickname"] = newName;

    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
}

//Слот обработки ответов сервера
void SettingsForm::onServerResponse()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "SettingsForm Response data:" << responseData;
    if (jsonObj.contains("status"))
    {
        QString status = jsonObj["status"].toString();

        if (status == "success" && jsonObj["type"].toString() == "update_login")
        {
            QMessageBox::information(this, tr("Успех"), tr("Логин успешно обновлён."));
            // Обновляем текущий логин
            login = loginEdit->text();
            loginEdit->setReadOnly(true);
            saveLoginButton->hide();
            changeLoginButton->show();
        }
        else if (status == "error" && jsonObj["type"].toString() == "update_login")
        {
            QString message = jsonObj.contains("message") ? jsonObj["message"].toString() :
                                  tr("Логин не соответствует правилам, либо уже используется.");
            QMessageBox::warning(this, tr("Ошибка"), message);
            // Возвращаем старое значение логина в поле, если ошибка
            loginEdit->setText(login);
        }
        if (status == "success" && jsonObj["type"].toString() == "update_nickname")
        {
            qDebug() << "nickname has been updated\n";
            QMessageBox::information(this, tr("Успех"), tr("Имя пользователя успешно обновлено."));
            nameEdit->setReadOnly(true);
            saveNameButton->hide();
            changeNameButton->show();
        }
        else if (status == "error" && jsonObj["type"].toString() == "update_nickname")
        {
            QString message = jsonObj["message"].toString();
            QMessageBox::warning(this, tr("Ошибка"), message);
        }
        if (status == "success" && jsonObj["type"].toString() == "check_nickname")
        {
            if (jsonObj.contains("nickname")) {
                nickname = jsonObj["nickname"].toString();
                nameEdit->setText(nickname);
            }
        }
        else if (status == "error" && jsonObj["type"].toString() == "check_nickname") {
            QString message = jsonObj.contains("message") ? jsonObj["message"].toString() : tr("Произошла ошибка.");
            QMessageBox::warning(this, tr("Ошибка"), "Данный логин уже используется.");

        }
        if (status == "success" &&  jsonObj["type"].toString() == "update_password") {
            QMessageBox::information(this, tr("Успех"), tr("Пароль успешно обновлён."));
        }
        else if (status == "error" &&  jsonObj["type"].toString() == "update_password") {
            QString message = jsonObj.contains("message") ? jsonObj["message"].toString() : tr("Произошла ошибка.");
            QMessageBox::warning(this, tr("Ошибка"), message);
        }
    }
}

//Подключение к сокету для отправки сообщений на сервер и получения ответов
void SettingsForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &SettingsForm::onServerResponse);
}

//Обработка нажатия кнопки "Назад"
void SettingsForm::handleBackClick()
{
    disconnect(socket, &QTcpSocket::readyRead, this, &SettingsForm::onServerResponse);
    //Испускает сигнал, который сообщит MainWindow, что нужно вернуться к MessengerForm
    emit backToMessengerFormRequested();
}

//Проверка логина
bool SettingsForm::loginContainsOnlyAllowedCharacters(const QString &login)
{
    QRegularExpression loginRegExp("^[A-Za-z\\d_-]+$"); //Регулярное выражение для допустимых символов в логине
    return login.contains(loginRegExp);
}

//Запрос имени (nickname)
void SettingsForm::requestNickname()
{
    if (!login.isEmpty())
    {
        QJsonObject request;
        request["type"] = "check_nickname";
        request["login"] = login;
        qDebug() << "Nickname requested\n";
        QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
        socket->write(requestData);
        socket->flush();
    }
}

//Проверка пароля
bool SettingsForm::passwordContainsRequiredCharacters(const QString &password)
{
    QRegularExpression upperCaseRegExp("[A-Z]"); //Регулярное выражение для заглавных букв
    QRegularExpression lowerCaseRegExp("[a-z]"); //Регулярное выражение для строчных букв
    QRegularExpression digitRegExp("\\d");       //Регулярное выражение для цифр
    QRegularExpression specialRegExp("[!@#$%^&*()_+=-]"); //Регулярное выражение для специальных символов

    return password.contains(upperCaseRegExp) &&
           password.contains(lowerCaseRegExp) &&
           password.contains(digitRegExp) &&
           password.contains(specialRegExp);
}

//Была нажата кнопка "Изменить пароль"
void SettingsForm::enablePasswordChange()
{
    //Создаем диалоговое окно для ввода паролей
    QDialog passwordDialog(this);
    passwordDialog.setWindowTitle(tr("Сменить пароль"));
    passwordDialog.setFixedSize(300, 200);

    //Поля ввода для текущего пароля, нового пароля и подтверждения нового пароля
    QLabel *currentPasswordLabel = new QLabel(tr("Введите пароль:"), &passwordDialog);
    QLineEdit *currentPasswordEdit = new QLineEdit(&passwordDialog);
    currentPasswordEdit->setEchoMode(QLineEdit::Password);
    currentPasswordEdit->setFixedWidth(200);

    QLabel *newPasswordLabel = new QLabel(tr("Введите новый пароль:"), &passwordDialog);
    QLineEdit *newPasswordEdit = new QLineEdit(&passwordDialog);
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    newPasswordEdit->setFixedWidth(200);

    QLabel *confirmPasswordLabel = new QLabel(tr("Повторите новый пароль:"), &passwordDialog);
    QLineEdit *confirmPasswordEdit = new QLineEdit(&passwordDialog);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setFixedWidth(200);

    //Добавляем кнопки ОК и Отмена
    QPushButton *okButton = new QPushButton(tr("ОК"), &passwordDialog);
    QPushButton *cancelButton = new QPushButton(tr("Отмена"), &passwordDialog);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addWidget(currentPasswordLabel);
    dialogLayout->addWidget(currentPasswordEdit);
    dialogLayout->addWidget(newPasswordLabel);
    dialogLayout->addWidget(newPasswordEdit);
    dialogLayout->addWidget(confirmPasswordLabel);
    dialogLayout->addWidget(confirmPasswordEdit);
    dialogLayout->addLayout(buttonLayout);

    passwordDialog.setLayout(dialogLayout);

    //Подключаем кнопку ОК
    connect(okButton, &QPushButton::clicked, [&]() {
        QString currentPassword = currentPasswordEdit->text();
        QString newPassword = newPasswordEdit->text();
        QString confirmPassword = confirmPasswordEdit->text();

        //Проверяем сложность нового пароля
        if(newPassword.length() < 8 || !passwordContainsRequiredCharacters(newPassword))
        {
            QMessageBox::warning(&passwordDialog, tr("Ошибка"), tr("Новый пароль не соответствует требованиям по сложности."));
            return;
        }

        //Проверяем совпадение новых паролей
        if(newPassword != confirmPassword)
        {
            QMessageBox::warning(&passwordDialog, tr("Ошибка"), tr("Новые пароли не совпадают."));
            return;
        }

        QJsonObject request;
        request["type"] = "update_password";
        request["login"] = login;
        QByteArray byteArrayPasswordSalt = (currentPassword + login).toUtf8();
        QByteArray hashedPassword = QCryptographicHash::hash(byteArrayPasswordSalt, QCryptographicHash::Sha512);
        request["current_password"] = QString(hashedPassword.toHex());
        QByteArray byteArrayNewPasswordSalt = (newPassword + login).toUtf8();
        QByteArray hashedNewPassword = QCryptographicHash::hash(byteArrayNewPasswordSalt, QCryptographicHash::Sha512);
        request["new_password"] = QString(hashedNewPassword.toHex());

        QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
        socket->write(requestData);
        socket->flush();

        passwordDialog.accept(); //Закрываем диалоговое окно после отправки запроса
    });

    //Подключаем кнопку Отмена
    connect(cancelButton, &QPushButton::clicked, [&]() {
        passwordDialog.reject(); //Закрываем диалоговое окно
    });

    passwordDialog.exec(); //Отображаем диалоговое окно
}


