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
    connect(backButton, &QPushButton::clicked, this, &SettingsForm::handleBackClick);
    nameEdit->setText(nickname);
    nameEdit->setReadOnly(true); // Поле для ввода, в котором нельзя писать

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

    if(newLogin == login) {
        saveLoginButton->hide();
        changeLoginButton->show();
        loginEdit->setReadOnly(true);
        QMessageBox::information(this, tr("Информация"), tr("Логин не был изменён."));
        return;
    }

    // Проверяем, что новый логин не пуст и содержит только допустимые символы
    if(newLogin.isEmpty() || !loginContainsOnlyAllowedCharacters(newLogin)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Указан недопустимый логин."));
        return;
    }

    // Запрос пароля у пользователя
    bool ok;
    QString password = QInputDialog::getText(this, tr("Подтверждение логина"),
                                             tr("Введите ваш пароль:"), QLineEdit::Password,
                                             "", &ok);

    // Если пользователь нажал OK и ввёл пароль
    if(ok && !password.isEmpty()) {

        // Создаём объект запроса
        QJsonObject request;
        request["type"] = "update_login";
        request["old_login"] = login; // Текущий логин пользователя
        request["new_login"] = newLogin; // Новый логин
        request["password"] = password; // Хэш пароля

        // Преобразуем запрос в JSON и отправляем его на сервер
        QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
        socket->write(requestData);
        socket->flush();

        // Пользовательский интерфейс обновляем после получения подтверждения от сервера
    } else if (ok) {
        // Если пользователь оставил поле пароля пустым и нажал OK
        QMessageBox::warning(this, tr("Ошибка"), tr("Пароль не может быть пустым."));
    }
    // В противном случае пользователь нажал "Отмена", и ничего делать не нужно
}


void SettingsForm::saveName() {
    QString newName = nameEdit->text();

    // Проверяем, что новое имя не пустое
    if(newName.isEmpty() || newName == "New user") {
        QMessageBox::warning(this, tr("Ошибка"), tr("Указано недопустимое имя."));
        return;
    }

    // Отправляем запрос на сервер для обновления имени
    QJsonObject request;
    request["type"] = "update_nickname";
    request["login"] = login; // Текущий логин пользователя, как идентификатор
    request["nickname"] = newName; // Новый nickname

    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
}


void SettingsForm::onServerResponse()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "SettingsForm Response data:" << responseData;
    if (jsonObj.contains("status"))
    {
        QString status = jsonObj["status"].toString();

        if (status == "success" && jsonObj["type"].toString() == "update_login") {
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
                nameEdit->setText(nickname);  // Устанавливаем полученный nickname
            }
        }
        else if (status == "error" && jsonObj["type"].toString() == "check_nickname") {
            QString message = jsonObj.contains("message") ? jsonObj["message"].toString() : tr("Произошла ошибка.");
            QMessageBox::warning(this, tr("Ошибка"), "Данный логин уже используется.");

        }
    }
}

void SettingsForm::connectSocket() {
    connect(socket, &QTcpSocket::readyRead, this, &SettingsForm::onServerResponse);
}

void SettingsForm::handleBackClick()
{
    // Отключаемся от сокета, чтобы не получать данные в этом виджете
    disconnect(socket, &QTcpSocket::readyRead, this, &SettingsForm::onServerResponse);

    // Испускаем сигнал, который сообщит MainWindow, что нужно вернуться к MessengerForm
    emit backToMessengerFormRequested();
}

bool SettingsForm::loginContainsOnlyAllowedCharacters(const QString &login)
{
    QRegularExpression loginRegExp("^[A-Za-z\\d_-]+$"); //Регулярное выражение для допустимых символов в логине
    return login.contains(loginRegExp);
}

void SettingsForm::requestNickname()
{
    if (!login.isEmpty()) {
        QJsonObject request;
        request["type"] = "check_nickname";
        request["login"] = login;
        qDebug() << "Nickname requested\n";
        QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
        socket->write(requestData);
        socket->flush();
    }
}
