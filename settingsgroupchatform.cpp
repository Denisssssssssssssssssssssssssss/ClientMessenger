#include "settingsgroupchatform.h"
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

//Подключение к сокету для отправки сообщений на сервер и получения ответов
void SettingsGroupChatForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &SettingsGroupChatForm::onServerResponse);
}

SettingsGroupChatForm::SettingsGroupChatForm(QTcpSocket *socket, QWidget *parent) : socket(socket)
{
    backButton = new QPushButton(tr("Назад"), this);
    backButton->setFixedWidth(120);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(backButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);

    connect(backButton, &QPushButton::clicked, this, &SettingsGroupChatForm::handleBackClick);

    setLayout(mainLayout);
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
    qDebug() << "SettingsGroupChatForm Response data:" << responseData;
}
