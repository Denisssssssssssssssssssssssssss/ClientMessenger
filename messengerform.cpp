#include "MessengerForm.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QByteArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QListWidgetItem>

MessengerForm::MessengerForm(QTcpSocket *socket, QString login, QWidget *parent) : QWidget(parent), socket(socket), login(login)
{
    //Создание элементов интерфейса
    settingsButton = new QPushButton(tr("Настройки"));
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText(tr("Поиск..."));
    chatList = new QListWidget();
    userList = new QListWidget();
    logOutButton = new QPushButton(tr("Выйти"));

    //Верхний слой с кнопками и полем поиска
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(searchEdit);

    //Основной слой компоновки
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(chatList);
    mainLayout->addWidget(userList);
    userList->hide();
    mainLayout->addWidget(logOutButton);

    //Инициализация контекстного меню
    contextMenu = new QMenu(this);
    deleteAction = new QAction(tr("Удалить"), this);
    contextMenu->addAction(deleteAction);

    //Подключение нажатий кнопок к слотам
    connect(settingsButton, &QPushButton::clicked, this, &MessengerForm::openSettings);
    connect(logOutButton, &QPushButton::clicked, this, &MessengerForm::logOut);
    connect(searchEdit, &QLineEdit::textChanged, this, &MessengerForm::onSearchTextChanged);
    connect(userList, &QListWidget::itemClicked, this, &MessengerForm::openChat);
    connect(chatList, &QListWidget::itemClicked, this, &MessengerForm::onChatListItemClicked);
    connect(deleteAction, &QAction::triggered, this, &MessengerForm::deleteChat);
    chatList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(chatList, &QListWidget::customContextMenuRequested, this, &MessengerForm::showContextMenu);
}

//Метод для отправки запроса на поиск пользователя
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

//Подключение к сокету для отправки сообщений на сервер и получения ответов
void MessengerForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &MessengerForm::onReadyRead);
}

//Обработка ответов от сервера
void MessengerForm::onReadyRead()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "MessengerForm Response data: " << jsonObj;
    if (jsonObj.contains("users") && jsonObj["users"].isArray())
    {
        QJsonArray usersArray = jsonObj["users"].toArray();
        updateUserList(usersArray);
    }
    else if (jsonObj.contains("chats") && jsonObj["chats"].isArray())
    {
        QJsonArray chatsArray = jsonObj["chats"].toArray();
        updateChatList(chatsArray);
    }
    else if(jsonObj.contains("type") && jsonObj["type"].toString() == "chat_update")
    {
        qDebug() << "Some chat updated.";
        requestChatList();
    }
    else if (jsonObj.contains("chat_id"))
    {
        QString chatId = jsonObj["chat_id"].toString();
        qDebug() << chatId;
        emit chatIdReceived(chatId); //Передаем chat_id через сигнал
    }
}

//Нажатие на кнопку "Настройки"
void MessengerForm::openSettings()
{
    disconnect(socket, nullptr, this, nullptr);
    emit settingsRequested();
}

//Если текст в строке поиска изменился
void MessengerForm::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty())
    {
        userList->hide();
        chatList->show();
    }
    else
    {
        findUsers();
    }
}

//Кнопка "Выйти"
void MessengerForm::logOut()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Выход"), tr("Вы уверены, что хотите выйти?"), QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        disconnect(socket, nullptr, this, nullptr);
        emit logoutRequested();
    }
}

//Нажатие на элемент chatList или userList
void MessengerForm::openChat(QListWidgetItem* item)
{
    QString otherUserLogin = item->data(Qt::UserRole).toString(); // Получаем login другого пользователя
    QString otherUserNickname = item->data(Qt::UserRole + 1).toString(); // Получаем nickname другого пользователя

    QJsonObject request {
        {"type", "get_or_create_chat"},
        {"login1", login},
        {"login2", otherUserLogin}
    };

    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
    searchEdit->clear();

    disconnect(this, &MessengerForm::chatIdReceived, nullptr, nullptr);
    connect(this, &MessengerForm::chatIdReceived, [this, otherUserLogin, otherUserNickname](const QString& chatId) {
        disconnect(this, &MessengerForm::chatIdReceived, nullptr, nullptr); //Отключаем временное подключение
        qDebug() << "chatId received in openChat: " << chatId;
        emit chatRequested(chatId, otherUserNickname);
    });
}

//Обновить список пользователей при поиске
void MessengerForm::updateUserList(QJsonArray users)
{
    userList->clear();
    for (const QJsonValue &value : users)
    {
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

//Обновить список чатов
void MessengerForm::updateChatList(QJsonArray chats)
{
    chatList->clear();
    for (const QJsonValue &value : chats)
    {
        QJsonObject chatObj = value.toObject();
        QString chatId = QString::number(chatObj["chat_id"].toInt());
        QString otherNickname = chatObj["other_nickname"].toString();
        int unreadCount = chatObj["unread_count"].toInt();

        QString chatTitle = otherNickname;
        if (unreadCount > 0)
        {
            chatTitle += " (Новое сообщение)";
        }

        QListWidgetItem *chatItem = new QListWidgetItem(chatTitle);
        chatItem->setData(Qt::UserRole, chatId);
        chatList->addItem(chatItem);
    }
}

//Нажатие на элемент списка чатов
void MessengerForm::onChatListItemClicked(QListWidgetItem *item)
{
    QString chatId = item->data(Qt::UserRole).toString();
    QString otherUserNickname = item->text(); //В данном случае текстом элемента будет nickname второго пользователя
    disconnect(socket, nullptr, this, nullptr);
    emit chatRequested(chatId, otherUserNickname);
}

//Запрос на получение списка чатов
void MessengerForm::requestChatList()
{
    QJsonObject request {
        {"type", "get_chat_list"},
        {"login", login}
    };
    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
}

//Показать контекстное меню
void MessengerForm::showContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = chatList->itemAt(pos);
    if (item)
    {
        contextMenu->exec(chatList->viewport()->mapToGlobal(pos));
    }
}

//Удаление чата
void MessengerForm::deleteChat()
{
    QListWidgetItem *item = chatList->currentItem();
    if (item)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Удалить чат"),
            tr("Вы уверены, что хотите удалить чат? Все сообщения удалятся у вас и вашего собеседника. Переписку нельзя будет восстановить."),
                QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            QString chatId = item->data(Qt::UserRole).toString();
            QJsonObject request {
                {"type", "delete_chat"},
                {"chat_id", chatId}
            };
            QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
            socket->write(requestData);
            socket->flush();
            delete item;
        }
    }
}
