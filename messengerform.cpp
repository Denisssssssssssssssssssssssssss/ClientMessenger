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
    connect(chatList, &QListWidget::itemClicked, this, &MessengerForm::onChatListItemClicked);
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
    } else if (jsonObj.contains("chats") && jsonObj["chats"].isArray()) {
        QJsonArray chatsArray = jsonObj["chats"].toArray();
        updateChatList(chatsArray);
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

void MessengerForm::updateChatList(QJsonArray chats)
{
    chatList->clear();
    for (const QJsonValue &value : chats) {
        QJsonObject chatObj = value.toObject();
        QString chatName = chatObj["chat_name"].toString();
        QString otherNickname = chatObj["other_nickname"].toString();
        QListWidgetItem *chatItem = new QListWidgetItem(otherNickname);
        chatItem->setData(Qt::UserRole, chatName); // Сохраняем chat_name в данных элемента, если потребуется
        chatList->addItem(chatItem);
    }
}


void MessengerForm::requestChatList()
{
    QJsonObject request{
        {"type", "get_chat_list"},
        {"login", login}
    };
    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
}

void MessengerForm::onChatListItemClicked(QListWidgetItem *item)
{
    QString chatName = item->data(Qt::UserRole).toString(); // Получаем chat_name

    // В этом случае предполагаем, что chat_name содержит имя чата в формате "login1login2"
    // Вы можете адаптировать этот код, если формат имени чата изменится
    QString otherUserNickname = item->text(); // В данном случае текстом элемента будет nickname второго пользователя
    emit chatRequested(otherUserNickname);
}
