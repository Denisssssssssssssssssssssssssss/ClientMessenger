#include "ChatForm.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QListWidgetItem>
#include <QJsonArray>

/*!
 * \brief Конструктор класса ChatForm.
 *
 * Создает экземпляр формы чата и инициализирует интерфейс.
 *
 * \param socket Указатель на сокет для сетевого взаимодействия.
 * \param login Логин пользователя.
 * \param chatId Идентификатор чата.
 * \param parent Указатель на родительский виджет (по умолчанию nullptr).
 */
ChatForm::ChatForm(QTcpSocket *socket, QString login, QString chatId, QWidget *parent)
    : QWidget(parent), socket(socket), login(login), chatId(chatId)
{
    // Создание элементов интерфейса
    backButton = new QPushButton(tr("Назад"));
    messageList = new QListWidget();
    messageEdit = new QLineEdit();
    messageEdit->setPlaceholderText(tr("Сообщение..."));
    sendButton = new QPushButton(tr("Отправить"));

    // Верхний слой с кнопками "Назад" и "Добавить в ЧС"
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(backButton, 0, Qt::AlignLeft);

    // Нижний слой с полем ввода сообщения и кнопкой "Отправить"
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(messageEdit);
    bottomLayout->addWidget(sendButton);

    // Основной слой компоновки
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(messageList);
    mainLayout->addLayout(bottomLayout);

    // Подключение нажатий кнопок к слотам
    connect(backButton, &QPushButton::clicked, this, &ChatForm::goBack);
    connect(sendButton, &QPushButton::clicked, this, &ChatForm::sendMessage);

    // Загрузка истории сообщений
    loadChatHistory();
}

/*!
 * \brief Обрабатывает нажатие кнопки "Назад".
 *
 * Этот слот отключает сокет и отправляет сигнал о запросе возврата на предыдущий экран.
 */
void ChatForm::goBack()
{
    disconnect(socket, nullptr, this, nullptr);
    emit backRequested();
}

/*!
 * \brief Обрабатывает нажатие кнопки "Добавить в черный список".
 *
 * Этот слот будет реализован позже.
 */
void ChatForm::addToBlacklist()
{
    // Coming soon
}

/*!
 * \brief Отправляет сообщение в чат.
 *
 * Этот слот собирает текст сообщения и отправляет его на сервер.
 */
void ChatForm::sendMessage()
{
    QString messageText = messageEdit->text().trimmed();
    if (messageText.isEmpty()) return;

    // Получение текущего времени в формате UTC
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

    // Отображение отправленного сообщения
    QString formattedTimestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
    appendMessageToList(messageText, formattedTimestamp, true);

    messageEdit->clear();
}

/*!
 * \brief Загружает историю переписки из чата.
 *
 * Этот метод отправляет запрос на сервер для получения истории сообщений.
 */
void ChatForm::loadChatHistory()
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

/*!
 * \brief Подключает сокет для получения сообщений от сервера.
 *
 * Этот метод устанавливает соединение с сокетом для обработки входящих данных.
 */
void ChatForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &ChatForm::onReadyRead);
}

/*!
 * \brief Обрабатывает ответы от сервера.
 *
 * Этот слот вызывается при получении данных из сокета и обрабатывает входящие сообщения.
 */
void ChatForm::onReadyRead()
{
    QByteArray responseData = socket->readAll();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

    if (!jsonDoc.isNull()) {
        QJsonObject jsonObj = jsonDoc.object();
        qDebug() << "ChatForm response data: " << jsonObj;

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

                    // Преобразование временной метки из UTC в локальное время клиента
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

                // Преобразование временной метки из UTC в локальное время клиента
                QDateTime timestampLocal = QDateTime::fromString(timestampUTC, Qt::ISODate).toLocalTime();
                QString formattedTimestamp = timestampLocal.toString("yyyy-MM-dd HH:mm");

                bool isOwnMessage = jsonObj["user_id"].toString() == login;
                appendMessageToList(messageText, formattedTimestamp, isOwnMessage);
            }
        }
    }
}

/*!
 * \brief Добавляет сообщение в виджет с сообщениями.
 *
 * Этот метод добавляет новое сообщение в список сообщений с временной меткой.
 *
 * \param message Текст сообщения.
 * \param timestamp Временная метка сообщения.
 * \param isOwnMessage Указывает, является ли сообщение собственным (true) или полученным (false).
 */
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
