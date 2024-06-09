#include "ChatForm.h"

ChatForm::ChatForm(QTcpSocket *socket, QString login, QWidget *parent) : QWidget(parent), socket(socket), login(login)
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
    // Логика для отправки сообщения
}

void ChatForm::connectSocket()
{
    //
}
