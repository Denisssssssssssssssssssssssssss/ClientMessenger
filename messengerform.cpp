#include "MessengerForm.h"

#include <QJsonDocument>
#include <QMessageBox>
#include <QByteArray>
#include <QJsonValue>
#include <QJsonObject>


MessengerForm::MessengerForm(QTcpSocket *socket, QString login, QWidget *parent) : QWidget(parent), socket(socket), login(login)
{
    // Создание элементов интерфейса
    settingsButton = new QPushButton(tr("Настройки"));
    findButton = new QPushButton(tr("Найти"));
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText(tr("Поиск..."));
    chatList = new QListWidget();
    userList = new QListWidget();
    logOutButton = new QPushButton(tr("Выйти"));

    // Верхний слой с кнопками и полем поиска
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(searchEdit);
    topLayout->addWidget(findButton);

    // Основной слой компоновки
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout); // Добавить слой с кнопками и поиском
    mainLayout->addWidget(chatList);  // Добавить виджет списка чатов
    mainLayout->addWidget(userList);
    userList->hide();
    mainLayout->addWidget(logOutButton); // Добавить кнопку "Выйти"

    // Подключение нажатий кнопок к слотам
    connect(settingsButton, &QPushButton::clicked, this, &MessengerForm::openSettings);
    connect(findButton, &QPushButton::clicked, this, &MessengerForm::findUsers);
    connect(logOutButton, &QPushButton::clicked, this, &MessengerForm::logOut);
}

void MessengerForm::openSettings()
{
    // Здесь код для открытия настроек
}

void MessengerForm::logOut()
{
    // Здесь код для выхода из учетной записи или приложения
}

void MessengerForm::findUsers() {
    QString searchText = searchEdit->text().trimmed();
    if (searchText.isEmpty()) {
        QMessageBox::warning(this, tr("Внимание"), tr("Поле поиска не может быть пустым."));
        return;
    }

    // Формируем JSON объект для отправки запроса
    QJsonObject request{
        {"type", "find_users"},
        {"searchText", searchText}
    };

    // Преобразование JSON объекта в строку и отправка
    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();

    // Показываем userList и скрываем chatList
    userList->show();
    chatList->hide();
}

void MessengerForm::updateUserList(QJsonArray users) {
    userList->clear(); // Очистка списка перед заполнением

    for (const QJsonValue &value : users) {
        QString user = value.toString();
        new QListWidgetItem(user, userList); // Добавление пользователя в список
    }
}

void MessengerForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &MessengerForm::onReadyRead);
}


void MessengerForm::onReadyRead()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj.contains("users") && jsonObj["users"].isArray()) {
        QJsonArray usersArray = jsonObj["users"].toArray();
        updateUserList(usersArray); // Передаем массив пользователей в слот
    }
}

