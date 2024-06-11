#include "ChatForm.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QListWidgetItem>
#include <QJsonArray>

ChatForm::ChatForm(QTcpSocket *socket, QString login, QString chatId, QWidget *parent) : QWidget(parent), socket(socket), login(login), chatId(chatId)
{
    // Создание элементов интерфейса
    backButton = new QPushButton(tr("Назад"));
    blacklistButton = new QPushButton(tr("Добавить в ЧС"));
    messageList = new QListWidget();
    messageEdit = new QLineEdit();
    messageEdit->setPlaceholderText(tr("Сообщение..."));
    sendButton = new QPushButton(tr("Отправить"));

    // Верхний слой с кнопками "Назад" и "Добавить в ЧС"
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(backButton, 0, Qt::AlignLeft);
    topLayout->addWidget(blacklistButton, 0, Qt::AlignRight);

    // Нижний слой с полем ввода сообщения и кнопкой "Отправить"
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(messageEdit);
    bottomLayout->addWidget(sendButton);

    // Основной слой компоновки
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout); // Добавить верхний слой
    mainLayout->addWidget(messageList); // Добавить виджет списка сообщений
    mainLayout->addLayout(bottomLayout); // Добавить нижний слой

    // Подключение нажатий кнопок к слотам
    connect(backButton, &QPushButton::clicked, this, &ChatForm::goBack);
    connect(blacklistButton, &QPushButton::clicked, this, &ChatForm::addToBlacklist);
    connect(sendButton, &QPushButton::clicked, this, &ChatForm::sendMessage);

    // Загрузка истории сообщений
    loadChatHistory();
}

void ChatForm::goBack()
{
    disconnect(socket, nullptr, this, nullptr);
    emit backRequested();
}

void ChatForm::addToBlacklist()
{
    // Логика для добавления в черный список
}

void ChatForm::sendMessage()
{
    QString messageText = messageEdit->text().trimmed();
    if (messageText.isEmpty()) return;

    // Получение текущего времени в формате UTC
    QDateTime currentTimeUTC = QDateTime::currentDateTimeUtc();
    QString timestampUTC = currentTimeUTC.toString(Qt::ISODate);

    // Создаем JSON-объект для отправки сообщения на сервер
    QJsonObject messageJson;
    messageJson["type"] = "send_message";
    messageJson["chat_id"] = chatId;
    messageJson["user_id"] = login;
    messageJson["message_text"] = messageText;
    messageJson["timestamp"] = timestampUTC; // Добавляем временную метку

    QByteArray requestData = QJsonDocument(messageJson).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();

    // Отображаем сообщение самостоятельно
    QString formattedTimestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
    appendMessageToList(messageText, formattedTimestamp, true);

    messageEdit->clear();
}

void ChatForm::loadChatHistory()
{
    // Запрос истории сообщений с сервера
    QJsonObject request{
        {"type", "get_chat_history"},
        {"chat_id", chatId}
    };
    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();
}

void ChatForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &ChatForm::onReadyRead);
}

void ChatForm::onReadyRead()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "ChatForm response data: " << jsonObj;
    if (jsonObj.contains("messages") && jsonObj["messages"].isArray()) {
        QJsonArray messagesArray = jsonObj["messages"].toArray();
        for (const QJsonValue &value : messagesArray) {
            QJsonObject messageObj = value.toObject();
            QString messageText = messageObj["message_text"].toString();
            QString timestampUTC = messageObj["timestamp"].toString();

            // Преобразование временной метки из UTC в локальное время клиента
            QDateTime timestampLocal = QDateTime::fromString(timestampUTC, Qt::ISODate).toLocalTime();
            QString formattedTimestamp = timestampLocal.toString("yyyy-MM-dd HH:mm");

            bool isOwnMessage = messageObj["user_id"].toString() == login;
            appendMessageToList(messageText, formattedTimestamp, isOwnMessage);
        }
    }
}

void ChatForm::appendMessageToList(const QString &message, const QString &timestamp, bool isOwnMessage)
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
