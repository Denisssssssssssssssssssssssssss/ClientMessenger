/**
 * /file settingsgroupchatform.cpp
 * /brief Реализация класса формы настроек группового чата.
 */

#include "settingsgroupchatform.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

/**
 * /brief Подключает сокет для отправки сообщений на сервер и получения ответов.
 */
void SettingsGroupChatForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &SettingsGroupChatForm::onServerResponse);
}

/**
 * /brief Конструктор SettingsGroupChatForm.
 * /param socket Указатель на QTcpSocket для работы с сетевыми соединениями.
 * /param login Логин пользователя.
 * /param chatId Идентификатор группового чата.
 * /param parent Указатель на родительский виджет.
 */
SettingsGroupChatForm::SettingsGroupChatForm(QTcpSocket *socket, QString login, QString chatId, QWidget *parent)
    : QWidget(parent), socket(socket), login(login), chatId(chatId)
{
    // Инициализация UI компонентов
    backButton = new QPushButton("Назад", this);
    participantsList = new QListWidget(this);
    availableUsersList = new QListWidget(this);
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText(tr("Поиск..."));

    connect(searchEdit, &QLineEdit::textChanged, this, &SettingsGroupChatForm::onSearchTextChanged);

    // Подключение сигналов кнопок к слотам
    connect(backButton, &QPushButton::clicked, this, &SettingsGroupChatForm::handleBackClick);

    // Установка макета
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *topLayout = new QHBoxLayout();

    topLayout->addWidget(backButton);
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(new QLabel(tr("Участники чата")));
    mainLayout->addWidget(participantsList);
    mainLayout->addWidget(new QLabel(tr("Доступные пользователи")));
    mainLayout->addWidget(searchEdit);
    mainLayout->addWidget(availableUsersList);

    setLayout(mainLayout);

    // Инициализация контекстного меню
    contextMenu = new QMenu(this);
    addAction = new QAction(tr("Добавить"), this);
    contextMenu->addAction(addAction);

    // Подключение сигналов для контекстного меню
    connect(addAction, &QAction::triggered, this, &SettingsGroupChatForm::onAddButtonClick);

    availableUsersList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(availableUsersList, &QListWidget::customContextMenuRequested, this, &SettingsGroupChatForm::showContextMenu);
}

/**
 * /brief Метод для показа контекстного меню.
 * /param pos Позиция, в которой должно появиться меню.
 */
void SettingsGroupChatForm::showContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = availableUsersList->itemAt(pos);
    if (item)
    {
        contextMenu->exec(availableUsersList->viewport()->mapToGlobal(pos));
    }
}

/**
 * /brief Обработка нажатия кнопки "Удалить" пользователя из списка участников.
 */
void SettingsGroupChatForm::onRemoveButtonClick()
{
    QListWidgetItem *selectedItem = participantsList->currentItem();
    if (selectedItem) {
        QString userName = selectedItem->text();
        availableUsersList->addItem(userName); // Возвращение пользователя в список доступных
        participantsList->takeItem(participantsList->row(selectedItem)); // Удаление пользователя из списка участников
    }
}

/**
 * /brief Обработка нажатия кнопки "Назад".
 */
void SettingsGroupChatForm::handleBackClick()
{
    disconnect(socket, &QTcpSocket::readyRead, this, &SettingsGroupChatForm::onServerResponse);
    emit backToMessengerFormRequested(); // Испускает сигнал для возврата к MessengerForm
}

/**
 * /brief Слот обработки ответов сервера.
 */
void SettingsGroupChatForm::onServerResponse()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "SettingsGroupChatForm Response data: " << jsonObj;

    // Обработка различных типов ответов
    if (jsonObj.contains("users") && jsonObj["users"].isArray()) {
        QJsonArray usersArray = jsonObj["users"].toArray();
        updateUserList(usersArray); // Обновление списка пользователей
    }
}

/**
 * /brief Метод для отправки запроса на поиск пользователя.
 */
void SettingsGroupChatForm::findUsers()
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

/**
 * /brief Обновляет список доступных пользователей при поиске.
 * /param users Массив JSON с пользователями.
 */
void SettingsGroupChatForm::updateUserList(QJsonArray users)
{
    availableUsersList->clear();
    for (const QJsonValue &value : users)
    {
        QJsonObject userObj = value.toObject();
        QString nickname = userObj["nickname"].toString();
        QString login = userObj["login"].toString();
        QListWidgetItem *userItem = new QListWidgetItem(nickname);
        userItem->setData(Qt::UserRole, login);  // Сохраняем login пользователя
        userItem->setData(Qt::UserRole + 1, nickname);  // Сохраняем nickname пользователя
        availableUsersList->addItem(userItem);
    }
}

/**
 * /brief Вызывается при изменении текста в строке поиска.
 * /param text Введённый текст для поиска.
 */
void SettingsGroupChatForm::onSearchTextChanged(const QString &text)
{
    if (!text.isEmpty())
    {
        findUsers(); // Запрос пользователей по введённому тексту
    }
}

/**
 * /brief Обработчик нажатия кнопки "Добавить" пользователя в чат.
 */
void SettingsGroupChatForm::onAddButtonClick()
{
    QListWidgetItem *selectedItem = availableUsersList->currentItem();

    if (selectedItem) {
        QString userLogin = selectedItem->data(Qt::UserRole).toString(); // Получаем логин пользователя
        QString userNickname = selectedItem->data(Qt::UserRole + 1).toString(); // Получаем никнейм пользователя

        // Создаем JSON запрос для добавления пользователя в чат
        QJsonObject request{
            {"type", "add_user_to_chat"},
            {"chatId", chatId},  // Добавляем chatId в запрос
            {"login", userLogin},
            {"nickname", userNickname}
        };

        QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
        socket->write(requestData);  // Отправляем запрос на сервер
        socket->flush();

        qDebug() << "Запрос на добавление пользователя отправлен: " << userLogin;

        participantsList->addItem(userNickname); // Добавление пользователя в список участников
        availableUsersList->takeItem(availableUsersList->row(selectedItem)); // Удаление пользователя из доступного списка
    }
}
