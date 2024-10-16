#include "groupchatform.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QListWidgetItem>
#include <QJsonArray>

GroupChatForm::GroupChatForm(QTcpSocket *socket, QString login, QString chatId, QWidget *parent) : QWidget(parent), socket(socket), login(login), chatId(chatId)
{
    //Создание элементов интерфейса
    backButton = new QPushButton(tr("Назад"));
    settingsButton = new QPushButton(tr("Настройки")); //Переименовать
    messageList = new QListWidget();
    messageEdit = new QLineEdit();
    messageEdit->setPlaceholderText(tr("Сообщение..."));
    sendButton = new QPushButton(tr("Отправить"));

    //Верхний слой с кнопками "Назад" и "Добавить в ЧС"
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(backButton, 0, Qt::AlignLeft);
    topLayout->addWidget(settingsButton, 0, Qt::AlignRight);

    //Нижний слой с полем ввода сообщения и кнопкой "Отправить"
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(messageEdit);
    bottomLayout->addWidget(sendButton);

    //Основной слой компоновки
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(messageList);
    mainLayout->addLayout(bottomLayout);

    //Подключение нажатий кнопок к слотам
    connect(backButton, &QPushButton::clicked, this, &GroupChatForm::goBack);
    connect(settingsButton, &QPushButton::clicked, this, &GroupChatForm::openSettings);
    connect(sendButton, &QPushButton::clicked, this, &GroupChatForm::sendMessage);

    //Загрузка истории сообщений
    loadChatHistory();
}

//Нажатие на кнопку назад
void GroupChatForm::goBack()
{
    disconnect(socket, nullptr, this, nullptr);
    emit backToMessengerFormRequested();
}

//Нажатие на кнопку настройки
void GroupChatForm::openSettings()
{
    disconnect(socket, nullptr, this, nullptr);
    emit openSettingsRequested();
}

//Отправка сообщения
void GroupChatForm::sendMessage()
{
    QString messageText = messageEdit->text().trimmed();
    if (messageText.isEmpty()) return;

    //Получение текущего времени в формате UTC
    QDateTime currentTimeUTC = QDateTime::currentDateTimeUtc();
    QString timestampUTC = currentTimeUTC.toString(Qt::ISODate);

    QJsonObject messageJson;
    messageJson["type"] = "send_message";
    messageJson["chat_id"] = chatId;
    messageJson["user_id"] = login;
    messageJson["message_text"] = messageText;
    messageJson["timestamp"] = timestampUTC;

    QByteArray requestData = QJsonDocument(messageJson).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();

    //Отображение отправленного сообщения
    QString formattedTimestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
    appendMessageToList(messageText, formattedTimestamp, true);

    messageEdit->clear();
}

//Загрузить историю переписки
void GroupChatForm::loadChatHistory()
{
    QJsonObject request{
        {"type", "get_chat_history"},
        {"chat_id", chatId},
        {"login", login}
    };
    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
}

//Подключение к сокету для отправки сообщений на сервер и получения ответов
void GroupChatForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &GroupChatForm::onReadyRead);
}

//Обработка ответов от сервера
void GroupChatForm::onReadyRead()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "GroupChatForm response data: " << jsonObj;

    if (jsonObj.contains("type"))
    {
        QString type = jsonObj["type"].toString();
        if (type == "get_chat_history" && jsonObj.contains("messages") && jsonObj["messages"].isArray())
        {
            QJsonArray messagesArray = jsonObj["messages"].toArray();
            for (const QJsonValue &value : messagesArray)
            {
                QJsonObject messageObj = value.toObject();
                QString messageText = messageObj["message_text"].toString();
                QString timestampUTC = messageObj["timestamp"].toString();

                //Преобразование временной метки из UTC в локальное время клиента
                QDateTime timestampLocal = QDateTime::fromString(timestampUTC, Qt::ISODate).toLocalTime();
                QString formattedTimestamp = timestampLocal.toString("yyyy-MM-dd HH:mm");

                bool isOwnMessage = messageObj["user_id"].toString() == login;
                appendMessageToList(messageText, formattedTimestamp, isOwnMessage);
            }
        }
        else if (type == "chat_update")
        {
            QString messageText = jsonObj["message_text"].toString();
            QString timestampUTC = jsonObj["timestamp"].toString();

            //Преобразование временной метки из UTC в локальное время клиента
            QDateTime timestampLocal = QDateTime::fromString(timestampUTC, Qt::ISODate).toLocalTime();
            QString formattedTimestamp = timestampLocal.toString("yyyy-MM-dd HH:mm");

            bool isOwnMessage = jsonObj["user_id"].toString() == login;
            appendMessageToList(messageText, formattedTimestamp, isOwnMessage);
        }
    }
}

//Добавление сообщения в виджет с сообщениями
void GroupChatForm::appendMessageToList(const QString &message, const QString &timestamp, bool isOwnMessage)
{
    QListWidgetItem *item = new QListWidgetItem;

    if (isOwnMessage) {
        item->setTextAlignment(Qt::AlignRight);
    } else {
        item->setTextAlignment(Qt::AlignLeft);
    }

    QString formattedMessage = message + "\n" + timestamp;
    item->setText(formattedMessage);
    messageList->addItem(item);
}
