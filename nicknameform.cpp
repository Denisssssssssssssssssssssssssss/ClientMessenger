#include "NicknameForm.h"
#include <QVBoxLayout>

NicknameForm::NicknameForm(QWidget *parent) : QWidget(parent)
{
    // Создание и настройка виджетов
    instructionsLabel = new QLabel(tr("Введите ваше имя"));
    QFont labelFont = instructionsLabel->font();
    labelFont.setPointSize(14);
    instructionsLabel->setFont(labelFont);

    nicknameEdit = new QLineEdit();
    nicknameEdit->setPlaceholderText(tr("Ввод..."));
    nicknameEdit->setFixedWidth(200);

    saveButton = new QPushButton(tr("Сохранить"));
    saveButton->setFixedWidth(120);

    // Размещение виджетов в слое компоновки
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(instructionsLabel);
    layout->addWidget(nicknameEdit);
    layout->addWidget(saveButton);
    layout->setAlignment(Qt::AlignCenter);

    // Подключение сигнала от кнопки к слоту saveNickname
    connect(saveButton, &QPushButton::clicked, this, &NicknameForm::saveNickname);
}

void NicknameForm::saveNickname()
{
    // Получение текста из поля ввода
    QString nickname = nicknameEdit->text();
    // Здесь может быть логика отправки никнейма на сервер или его сохранения локально

    // Предполагается, что вы реализуете эту функцию в соответствии с потребностями вашего приложения
}
