#include "settingsgroupchatform.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

//Подключение к сокету для отправки сообщений на сервер и получения ответов
void SettingsGroupChatForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &SettingsGroupChatForm::onServerResponse);
}

SettingsGroupChatForm::SettingsGroupChatForm(QTcpSocket *socket, QString login, QWidget *parent)
    : QWidget(parent), socket(socket), login(login)
{
    // Initialize UI components
    backButton = new QPushButton("Назад", this);
    participantsList = new QListWidget(this);
    availableUsersList = new QListWidget(this);
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText(tr("Поиск..."));
    connect(searchEdit, &QLineEdit::textChanged, this, &SettingsGroupChatForm::onSearchTextChanged);

    // Connect button signals to slots
    connect(backButton, &QPushButton::clicked, this, &SettingsGroupChatForm::handleBackClick);

    // Set up layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create a horizontal layout for the top section
    QHBoxLayout *topLayout = new QHBoxLayout();

    // Add the back button to the top layout
    topLayout->addWidget(backButton);


    // Add stretchable space to push other widgets down
    topLayout->addStretch();

    // Add top layout to main layout
    mainLayout->addLayout(topLayout);

    mainLayout->addWidget(new QLabel(tr("Участники чата")));

    // Add other widgets to the main layout
    mainLayout->addWidget(participantsList);
    mainLayout->addWidget(new QLabel(tr("Доступные пользователи")));
    mainLayout->addWidget(searchEdit);
    mainLayout->addWidget(availableUsersList);

    // Create a horizontal layout for add/remove buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // Add button layout to main layout
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void SettingsGroupChatForm::onAddButtonClick()
{
    QListWidgetItem *selectedItem = availableUsersList->currentItem();
    if (selectedItem) {
        QString userName = selectedItem->text();
        participantsList->addItem(userName); // Добавление пользователя в список участников
        availableUsersList->takeItem(availableUsersList->row(selectedItem)); // Удаление пользователя из доступного списка
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
