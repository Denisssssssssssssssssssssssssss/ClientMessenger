/**
 * /file NicknameForm.cpp
 * /brief Реализация класса формы для обновления имени пользователя.
 */

#include "NicknameForm.h"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>

/**
 * /brief Конструктор NicknameForm.
 * /param socket Указатель на QTcpSocket для работы с сетевыми соединениями.
 * /param login Логин пользователя.
 * /param parent Указатель на родительский виджет.
 */
NicknameForm::NicknameForm(QTcpSocket *socket, QString login, QWidget *parent) : QWidget(parent), socket(socket), login(login)
{
    // Создание и настройка виджетов
    instructionsLabel = new QLabel(tr("Введите ваше имя"));
    QFont labelFont = instructionsLabel->font();
    labelFont.setPointSize(14);
    instructionsLabel->setFont(labelFont);
    instructionsLabel->setAlignment(Qt::AlignCenter);

    nicknameEdit = new QLineEdit();
    nicknameEdit->setPlaceholderText(tr("Ввод..."));
    nicknameEdit->setFixedWidth(200);
    nicknameEdit->setAlignment(Qt::AlignCenter);

    saveButton = new QPushButton(tr("Сохранить"));
    saveButton->setFixedWidth(200);

    // Размещение виджетов в слое компоновки
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(instructionsLabel);
    layout->addWidget(nicknameEdit);
    layout->addWidget(saveButton);
    layout->setAlignment(Qt::AlignCenter);

    // Подключение сигнала от кнопки к слоту saveNickname
    connect(saveButton, &QPushButton::clicked, this, &NicknameForm::saveNickname);
}

/**
 * /brief Слот для сохранения имени пользователя.
 * Проверяет введенное имя и отправляет запрос на сервер для обновления имени.
 */
void NicknameForm::saveNickname()
{
    QString nickname = nicknameEdit->text().trimmed();

    if (nickname.isEmpty())
    {
        return;
    }

    if (nickname == "New user")
    {
        QMessageBox::warning(this, tr("Ошибка"), tr("Введено недопустимое имя!"));
        return;
    }

    QJsonObject request;
    request["type"] = "update_nickname";
    request["login"] = login;
    request["nickname"] = nickname;

    QByteArray requestData = QJsonDocument(request).toJson();
    socket->write(requestData);
    socket->flush();
}

/**
 * /brief Обработка ответа от сервера.
 * Выводит сообщение о результате обновления имени пользователя.
 */
void NicknameForm::handleServerResponse()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if(jsonObj["status"].toString() == "success")
    {
        QMessageBox::information(this, tr("Успех"), tr("Имя пользователя успешно обновлено!"));
        disconnect(socket, nullptr, this, nullptr);
        emit nicknameUpdated();
    }
    else
    {
        QMessageBox::critical(this, tr("Ошибка"), jsonObj["message"].toString());
    }
}

/**
 * /brief Подключает сокет для отправки сообщений на сервер и получения ответов.
 */
void NicknameForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &NicknameForm::handleServerResponse);
}
