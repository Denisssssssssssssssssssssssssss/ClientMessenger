#include "settingsgroupchatform.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

//Подключение к сокету для отправки сообщений на сервер и получения ответов
void SettingsGroupChatForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &SettingsGroupChatForm::onServerResponse);
}

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

// Метод для показа контекстного меню
void SettingsGroupChatForm::showContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = availableUsersList->itemAt(pos);
    if (item)
    {
        contextMenu->exec(availableUsersList->viewport()->mapToGlobal(pos));
    }
}

// Обработка нажатия кнопки "Удалить"
void SettingsGroupChatForm::onRemoveButtonClick()
{
    QListWidgetItem *selectedItem = participantsList->currentItem();
    if (selectedItem) {
        QString userName = selectedItem->text();
        availableUsersList->addItem(userName); // Возвращение пользователя в список доступных
        participantsList->takeItem(participantsList->row(selectedItem)); // Удаление пользователя из списка участников
    }
}

//Обработка нажатия кнопки "Назад"
void SettingsGroupChatForm::handleBackClick()
{
    disconnect(socket, &QTcpSocket::readyRead, this, &SettingsGroupChatForm::onServerResponse);
    //Испускает сигнал, который сообщит MainWindow, что нужно вернуться к MessengerForm
    emit backToMessengerFormRequested();
}

void SettingsGroupChatForm::onServerResponse()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "SettingsGroupChatForm Response data: " << jsonObj;

    // Handle different types of responses
    if (jsonObj.contains("users") && jsonObj["users"].isArray()) {
        QJsonArray usersArray = jsonObj["users"].toArray();
        updateUserList(usersArray);
    }
}

//Метод для отправки запроса на поиск пользователя
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

//Обновить список пользователей при поиске
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
        userItem->setData(Qt::UserRole + 2, login);  // Дублируем login пользователя в другом поле (для проверки корректности)
        availableUsersList->addItem(userItem);
    }
}


//Обработка ответов от сервера
void SettingsGroupChatForm::onReadyRead()
{

}

//Если текст в строке поиска изменился
void SettingsGroupChatForm::onSearchTextChanged(const QString &text)
{
    if (!text.isEmpty())
    {
        findUsers();
    }
}

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


