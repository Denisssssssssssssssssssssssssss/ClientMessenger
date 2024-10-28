/**
 * /file registrationform.cpp
 * /brief Реализация класса формы регистрации пользователей.
 */

#include "registrationform.h"

#include <QPixmap>
#include <QDir>
#include <QMessageBox>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>

/**
 * /brief Конструктор RegistrationForm.
 * /param socket Указатель на QTcpSocket для работы с сетевыми соединениями.
 * /param parent Указатель на родительский виджет.
 */
RegistrationForm::RegistrationForm(QTcpSocket *socket, QWidget *parent) : socket(socket)
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
    titleFont.setPointSize(14);
    titleLabel->setFont(titleFont);

    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText(tr("Введите логин..."));
    loginEdit->setFixedWidth(200);

    // Настройка виджетов для пароля
    opendEyeLabelPass = new QLabel(this);
    closedEyeLabelPass = new QLabel(this);
    QPixmap opendEyePixmap(":/images/icon-eye-opened.png");
    QPixmap closedEyePixmap(":/images/icon-eye-closed.png");
    opendEyeLabelPass->setPixmap(opendEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    closedEyeLabelPass->setPixmap(closedEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    opendEyeLabelPass->hide();

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText(tr("Введите пароль..."));
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedWidth(200);

    opendEyeLabelPassAgain = new QLabel(this);
    closedEyeLabelPassAgain = new QLabel(this);
    opendEyeLabelPassAgain->setPixmap(opendEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    closedEyeLabelPassAgain->setPixmap(closedEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    opendEyeLabelPassAgain->hide();

    // Расположение виджетов для ввода пароля
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    passwordLayout->addStretch(5);
    passwordLayout->addWidget(passwordEdit);
    passwordLayout->addWidget(closedEyeLabelPass);
    passwordLayout->addWidget(opendEyeLabelPass);
    passwordLayout->addStretch(4);

    passwordEditAgain = new QLineEdit();
    passwordEditAgain->setPlaceholderText(tr("Введите пароль еще раз..."));
    passwordEditAgain->setEchoMode(QLineEdit::Password);
    passwordEditAgain->setFixedWidth(200);

    QHBoxLayout *passwordAgainLayout = new QHBoxLayout();
    passwordAgainLayout->addStretch(5);
    passwordAgainLayout->addWidget(passwordEditAgain);
    passwordAgainLayout->addWidget(closedEyeLabelPassAgain);
    passwordAgainLayout->addWidget(opendEyeLabelPassAgain);
    passwordAgainLayout->addStretch(4);

    // Установка фильтров событий для виджетов
    opendEyeLabelPass->installEventFilter(this);
    closedEyeLabelPass->installEventFilter(this);
    opendEyeLabelPassAgain->installEventFilter(this);
    closedEyeLabelPassAgain->installEventFilter(this);

    registerButton = new QPushButton(tr("Зарегистрироваться"));
    registerButton->setFixedWidth(120);

    // Главный макет формы
    QVBoxLayout *centerLayout = new QVBoxLayout();
    centerLayout->addStretch(1);
    centerLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(loginEdit, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addLayout(passwordLayout);
    centerLayout->addSpacing(10);
    centerLayout->addLayout(passwordAgainLayout);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(registerButton, 0, Qt::AlignCenter);
    centerLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->setStretchFactor(topLayout, 1);
    mainLayout->addLayout(centerLayout);
    mainLayout->setStretchFactor(centerLayout, 9);
    setLayout(mainLayout);

    // Подключение сигналов к слотам
    connect(backButton, &QPushButton::clicked, this, &RegistrationForm::backButtonClicked);
    connect(registerButton, &QPushButton::clicked, this, &RegistrationForm::registerUser);
}

/**
 * /brief Фильтр событий для управления отображением пароля.
 * /param target Указатель на объект, который должен обрабатывать событие.
 * /param event Указатель на событие.
 * /return Признак обработки события (true, если обработано).
 */
bool RegistrationForm::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (target == closedEyeLabelPass || target == opendEyeLabelPass)
        {
            bool isPasswordVisible = passwordEdit->echoMode() == QLineEdit::Normal;
            passwordEdit->setEchoMode(isPasswordVisible ? QLineEdit::Password : QLineEdit::Normal);
            closedEyeLabelPass->setVisible(isPasswordVisible);
            opendEyeLabelPass->setVisible(!isPasswordVisible);
            return true;
        }
        else if (target == closedEyeLabelPassAgain || target == opendEyeLabelPassAgain)
        {
            bool isPasswordVisibleAgain = passwordEditAgain->echoMode() == QLineEdit::Normal;
            passwordEditAgain->setEchoMode(isPasswordVisibleAgain ? QLineEdit::Password : QLineEdit::Normal);
            closedEyeLabelPassAgain->setVisible(isPasswordVisibleAgain);
            opendEyeLabelPassAgain->setVisible(!isPasswordVisibleAgain);
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        if (target == imageLabel)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                onImageLabelClicked();
                return true;
            }
        }
    }
    return QWidget::eventFilter(target, event);
}

/**
 * /brief Обработчик нажатия на изображение помощи.
 */
void RegistrationForm::onImageLabelClicked()
{
    QMessageBox::information(this, "Информация", "Логин может состоять из символов латиницы любого регистра, цифр, тире и нижнего подчеркивания.\n"
                                                 "Логин должен состоять как минимум из 1 символа, как максимум из 255 символов.\n"
                                                 "Логин должен быть уникальным.\n"
                                                 "Пароль должен состоять из символов латиницы разного регистра, "
                                                 "цифр и специальных символов.\nДлина пароля не может быть менее 8 символов и\n"
                                                 "не может быть более 255 символов.");
}

/**
 * /brief Обработчик нажатия на кнопку "Назад".
 */
void RegistrationForm::backButtonClicked()
{
    loginEdit->clear();
    passwordEdit->clear();
    passwordEditAgain->clear();
    disconnect(socket, nullptr, this, nullptr);
    emit backRequested();
}

/**
 * /brief Обработчик нажатия на кнопку "Зарегистрироваться".
 */
void RegistrationForm::registerUser()
{
    QString login = loginEdit->text();
    QString password = passwordEdit->text();
    QString passwordAgain = passwordEditAgain->text();

    if (!login.length() || !password.length() || !passwordAgain.length())
    {
        QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены.");
        return;
    }
    if (!loginContainsOnlyAllowedCharacters(login))
    {
        QMessageBox::warning(this, "Ошибка", "В логине использованы запрещенные символы.");
        return;
    }
    if (password != passwordAgain)
    {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают.");
        return;
    }
    if (password.length() < 8 || !passwordContainsRequiredCharacters(password))
    {
        QMessageBox::warning(this, "Ошибка", "Пароль не соответствует требованиям безопасности.");
        return;
    }

    // Хеширование пароля
    QByteArray byteArrayPasswordSalt = (password + login).toUtf8();
    QByteArray hashedPassword = QCryptographicHash::hash(byteArrayPasswordSalt, QCryptographicHash::Sha512).toHex();

    QJsonObject registrationRequest;
    registrationRequest["type"] = "register";
    registrationRequest["login"] = login;
    registrationRequest["password"] = QString(hashedPassword);

    QByteArray dataArray = QJsonDocument(registrationRequest).toJson(QJsonDocument::Compact);

    socket->write(dataArray);
    socket->flush();
}

/**
 * /brief Проверяет, содержит ли пароль необходимые символы.
 * /param password Пароль для проверки.
 * /return Признак, соответствует ли пароль требованиям (true, если соответствует).
 */
bool RegistrationForm::passwordContainsRequiredCharacters(const QString &password)
{
    QRegularExpression upperCaseRegExp("[A-Z]");
    QRegularExpression lowerCaseRegExp("[a-z]");
    QRegularExpression digitRegExp("\\d");
    QRegularExpression specialRegExp("[!/#$%^&*()_+=-]");

    return password.contains(upperCaseRegExp) &&
           password.contains(lowerCaseRegExp) &&
           password.contains(digitRegExp) &&
           password.contains(specialRegExp);
}

/**
 * /brief Проверяет, содержит ли логин только разрешенные символы.
 * /param login Логин для проверки.
 * /return Признак, содержит ли логин только разрешенные символы (true, если содержит).
 */
bool RegistrationForm::loginContainsOnlyAllowedCharacters(const QString &login)
{
    QRegularExpression loginRegExp("^[A-Za-z\\d_-]+$");
    return login.contains(loginRegExp);
}

/**
 * /brief Обработка ответов от сервера.
 * Выводит сообщения о статусе регистрации.
 */
void RegistrationForm::handleServerResponse()
{
    QByteArray responseData = socket->readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(responseData));
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj.contains("status") && jsonObj["status"].toString() == "success")
    {
        QMessageBox::information(this, tr("Регистрация успешна"), tr("Регистрация прошла успешно!"));
        loginEdit->clear();
        passwordEdit->clear();
        passwordEditAgain->clear();
        disconnect(socket, nullptr, this, nullptr);
        emit backRequested();  // Предполагаем, что этот сигнал приводит к отображению формы входа
    }
    else
    {
        QMessageBox::critical(this, tr("Ошибка регистрации"), tr("Данный логин уже используется. Придумайте другой."));
        loginEdit->clear();
        passwordEdit->clear();
        passwordEditAgain->clear();
    }
}

/**
 * /brief Подключает сокет для отправки сообщений на сервер и получения ответов.
 */
void RegistrationForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &RegistrationForm::handleServerResponse);
}
