#include "registrationform.h"

#include <QPixmap>
#include <QDir>
#include <QMessageBox>

RegistrationForm::RegistrationForm(QWidget *parent)
{
    imageLabel = new QLabel(this);
    QPixmap pixmap(":/images/icon-info.png");
    QPixmap scaledPixmap = pixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(scaledPixmap);
    imageLabel->installEventFilter(this);

    backButton = new QPushButton(tr("Назад"));
    backButton->setFixedWidth(120);
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(backButton, 0, Qt::AlignTop | Qt::AlignLeft);
    topLayout->addWidget(imageLabel, 0, Qt::AlignTop | Qt::AlignRight);
    titleLabel = new QLabel(tr("Регистрация"));
    QFont titleFont = titleLabel->font();
    titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleLabel->setFont(titleFont);
    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText(tr("Введите логин..."));
    loginEdit->setFixedWidth(200);
    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText(tr("Введите пароль..."));
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedWidth(200);
    passwordEditAgain = new QLineEdit();
    passwordEditAgain->setPlaceholderText(tr("Введите пароль еще раз..."));
    passwordEditAgain->setEchoMode(QLineEdit::Password);
    passwordEditAgain->setFixedWidth(200);
    registerButton = new QPushButton(tr("Зарегистрироваться"));
    registerButton->setFixedWidth(120);
    QVBoxLayout *centerLayout = new QVBoxLayout();
    centerLayout->addStretch(1); //Начало макета с растяжимым пространством
    centerLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(loginEdit, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(passwordEdit, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(passwordEditAgain, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(registerButton, 0, Qt::AlignCenter);
    centerLayout->addStretch(1); //Конец макета с растяжимым пространством
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->setStretchFactor(topLayout, 1); //Меньший вес для topLayout
    mainLayout->addLayout(centerLayout);
    mainLayout->setStretchFactor(centerLayout, 9); //Больший вес для centerLayout
    setLayout(mainLayout);
}

bool RegistrationForm::eventFilter(QObject *target, QEvent *event)
{
    if (target == imageLabel && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            onImageLabelClicked(); // Вызовите слот при клике левой кнопкой мыши
            return true; // Возвращаем true, чтобы указать, что событие обработано
        }
    }
    return QWidget::eventFilter(target, event); // Для всех остальных случаев вызовите базовую реализацию
}

void RegistrationForm::onImageLabelClicked()
{
    QMessageBox::information(this, "Информация", "Здесь ваш информационный текст");
}
