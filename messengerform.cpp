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

void MessengerForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &MessengerForm::onReadyRead);
}

void MessengerForm::onReadyRead()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "MessengerForm Response data: " << jsonObj;
    if (jsonObj.contains("users") && jsonObj["users"].isArray()) {
        QJsonArray usersArray = jsonObj["users"].toArray();
        updateUserList(usersArray);
    } else if (jsonObj.contains("chats") && jsonObj["chats"].isArray()) {
        QJsonArray chatsArray = jsonObj["chats"].toArray();
        updateChatList(chatsArray);
    } else if (jsonObj.contains("chat_id")) {
        QString chatId = jsonObj["chat_id"].toString();
        qDebug() << chatId;
        emit chatIdReceived(chatId); // Передаем chat_id через сигнал
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

void MessengerForm::openChat(QListWidgetItem* item)
{
    QString otherUserLogin = item->data(Qt::UserRole).toString(); // Получаем login другого пользователя
    QString otherUserNickname = item->data(Qt::UserRole + 1).toString(); // Получаем nickname другого пользователя

    // Отправляем запрос на создание или получение чата
    QJsonObject request{
        {"type", "get_or_create_chat"},
        {"login1", login},
        {"login2", otherUserLogin}
    };

    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
    searchEdit->clear();

    // Убедимся, что временные подключение сигнала правильно отключается до его переназначения
    disconnect(this, &MessengerForm::chatIdReceived, nullptr, nullptr);
    connect(this, &MessengerForm::chatIdReceived, [this, otherUserLogin, otherUserNickname](const QString& chatId) {
        disconnect(this, &MessengerForm::chatIdReceived, nullptr, nullptr); // отключаем временное подключение
        qDebug() << "chatId received in openChat: " << chatId;
        emit chatRequested(chatId, otherUserNickname);
    });
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
        userItem->setData(Qt::UserRole + 2, login);  // Дублируем login пользователя в другом поле (для проверки корректности)
        userList->addItem(userItem);
    }
    chatList->hide();
    userList->show();
}

void MessengerForm::updateChatList(QJsonArray chats)
{
    chatList->clear();
    for (const QJsonValue &value : chats) {
        QJsonObject chatObj = value.toObject();
        QString chatId = QString::number(chatObj["chat_id"].toInt()); // Получаем chat_id и преобразуем в строку
        QString otherNickname = chatObj["other_nickname"].toString();
        QListWidgetItem *chatItem = new QListWidgetItem(otherNickname);
        chatItem->setData(Qt::UserRole, chatId); // Сохраняем chat_id в данных элемента
        chatList->addItem(chatItem);
    }
}

void MessengerForm::onChatListItemClicked(QListWidgetItem *item)
{
    QString chatId = item->data(Qt::UserRole).toString(); // Получаем chat_id
    QString otherUserNickname = item->text(); // В данном случае текстом элемента будет nickname второго пользователя
    disconnect(socket, nullptr, this, nullptr);
    emit chatRequested(chatId, otherUserNickname);
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

