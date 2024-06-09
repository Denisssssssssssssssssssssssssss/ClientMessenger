#include "MessengerForm.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QByteArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QListWidgetItem>

MessengerForm::MessengerForm(QTcpSocket *socket, QString login, QWidget *parent) : QWidget(parent), socket(socket), login(login)
{
    // Создание элементов интерфейса
    settingsButton = new QPushButton(tr("Настройки"));
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText(tr("Поиск..."));
    chatList = new QListWidget();
    userList = new QListWidget();
    logOutButton = new QPushButton(tr("Выйти"));

    // Верхний слой с кнопками и полем поиска
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(searchEdit);

    // Основной слой компоновки
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout); // Добавить слой с кнопками и поиском
    mainLayout->addWidget(chatList);  // Добавить виджет списка чатов
    mainLayout->addWidget(userList);
    userList->hide();
    mainLayout->addWidget(logOutButton); // Добавить кнопку "Выйти"

    // Подключение нажатий кнопок к слотам
    connect(settingsButton, &QPushButton::clicked, this, &MessengerForm::openSettings);
    connect(logOutButton, &QPushButton::clicked, this, &MessengerForm::logOut);
    connect(searchEdit, &QLineEdit::textChanged, this, &MessengerForm::onSearchTextChanged);
    connect(userList, &QListWidget::itemClicked, this, &MessengerForm::openChat);
}

void MessengerForm::findUsers()
{
    QString searchText = searchEdit->text().trimmed();
    QJsonObject request{
        {"type", "find_users"},
        {"searchText", searchText},
        {"login", login}
    };
    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
}

void MessengerForm::updateUserList(QJsonArray users)
{
    userList->clear();
    for (const QJsonValue &value : users) {
        QJsonObject userObj = value.toObject();
        QString nickname = userObj["nickname"].toString();
        QString login = userObj["login"].toString();
        QListWidgetItem *userItem = new QListWidgetItem(nickname);
        userItem->setData(Qt::UserRole, login);  // Сохраняем login пользователя
        userItem->setData(Qt::UserRole + 1, nickname);  // Сохраняем nickname пользователя
        userList->addItem(userItem);
    }
    chatList->hide();
    userList->show();
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
        updateUserList(usersArray);
    }
}

void MessengerForm::openSettings()
{
    disconnect(socket, nullptr, this, nullptr);
    emit settingsRequested();
}

void MessengerForm::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        userList->hide();
        chatList->show();
    } else {
        findUsers();
    }
}

void MessengerForm::logOut()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Выход"), tr("Вы уверены, что хотите выйти?"), QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        disconnect(socket, nullptr, this, nullptr);
        emit logoutRequested();
    }
}

void MessengerForm::openChat(QListWidgetItem *item)
{
    QString otherUserLogin = item->data(Qt::UserRole).toString(); // Получаем login другого пользователя
    QString otherUserNickname = item->data(Qt::UserRole + 1).toString(); // Получаем nickname другого пользователя
    QJsonObject request = {
        {"type", "create_chat"},
        {"user1", login},
        {"user2", otherUserLogin}
    };
    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
    emit chatRequested(otherUserNickname);
}
