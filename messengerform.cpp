#include "MessengerForm.h"

#include <QJsonDocument>
#include <QMessageBox>
#include <QByteArray>
#include <QJsonValue>
#include <QJsonObject>


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
}

void MessengerForm::findUsers() {
    QString searchText = searchEdit->text().trimmed();

    // Формируем JSON объект для отправки запроса
    QJsonObject request{
        {"type", "find_users"},
        {"searchText", searchText},
        {"login", login} // Добавляем логин пользователя, отправляющего запрос
    };

    // Преобразование JSON объекта в строку и отправка
    QByteArray requestData = QJsonDocument(request).toJson(QJsonDocument::Compact);
    socket->write(requestData);
    socket->flush();

    // Показываем userList и скрываем chatList
    userList->show();
    chatList->hide();
}

void MessengerForm::updateUserList(QJsonArray users) {

    userList->clear();

    for (const QJsonValue &value : users) {
        QString user = value.toString();
        new QListWidgetItem(user, userList); // Добавление пользователя в список
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
        updateUserList(usersArray); // Передаем массив пользователей в слот
    }
}

void MessengerForm::openSettings()
{
    disconnect(socket, nullptr, this, nullptr);
    emit settingsRequested(); // Испускаем сигнал, который должен обрабатываться MainWindow
}

void MessengerForm::onSearchTextChanged(const QString &text) {
    if (text.isEmpty()) {
        userList->hide();
        chatList->show();
    } else {
        // Если это не нужно, так как список пользователей
        // обновляется только после нажатия на кнопку "Найти",
        // то данный блок кода может быть опущен.
        findUsers();  // Если вы хотите сразу искать, как только пользователь начинает вводить текст
    }
}

void MessengerForm::logOut()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Выход"),
                                                              tr("Вы уверены, что хотите выйти?"),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        disconnect(socket, nullptr, this, nullptr);
        emit logoutRequested(); // Испускаем сигнал, что пользователь хочет выйти
    }
}
