#include "MessengerForm.h"

MessengerForm::MessengerForm(QWidget *parent) : QWidget(parent)
{
    // Создание элементов интерфейса
    settingsButton = new QPushButton(tr("Настройки"));
    findButton = new QPushButton(tr("Найти"));
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText(tr("Поиск..."));
    chatList = new QListWidget();
    logOutButton = new QPushButton(tr("Выйти"));

    // Верхний слой с кнопками и полем поиска
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(settingsButton);
    topLayout->addStretch(); // Добавить пространство между кнопками и полем поиска
    topLayout->addWidget(searchEdit);
    topLayout->addStretch(); // Добавить пространство между полем поиска и концом слоя
    topLayout->addWidget(findButton);

    // Основной слой компоновки
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout); // Добавить слой с кнопками и поиском
    mainLayout->addWidget(chatList);  // Добавить виджет списка чатов
    mainLayout->addWidget(logOutButton); // Добавить кнопку "Выйти"

    // Подключение нажатий кнопок к слотам
    connect(settingsButton, &QPushButton::clicked, this, &MessengerForm::openSettings);
    connect(findButton, &QPushButton::clicked, this, &MessengerForm::findChats);
    connect(logOutButton, &QPushButton::clicked, this, &MessengerForm::logOut);
}

void MessengerForm::openSettings()
{
    // Здесь код для открытия настроек
}

void MessengerForm::findChats()
{
    // Здесь код для поиска чатов
}

void MessengerForm::logOut()
{
    // Здесь код для выхода из учетной записи или приложения
}
