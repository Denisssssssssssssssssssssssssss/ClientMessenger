#include "registrationform.h"

#include <QPixmap>
#include <QDir>
#include <QMessageBox>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>

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
    titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleLabel->setFont(titleFont);
    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText(tr("Введите логин..."));
    loginEdit->setFixedWidth(200);

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

    opendEyeLabelPass->installEventFilter(this);
    closedEyeLabelPass->installEventFilter(this);
    opendEyeLabelPassAgain->installEventFilter(this);
    closedEyeLabelPassAgain->installEventFilter(this);

    registerButton = new QPushButton(tr("Зарегистрироваться"));
    registerButton->setFixedWidth(120);
    QVBoxLayout *centerLayout = new QVBoxLayout();
    centerLayout->addStretch(1); //Начало макета с растяжимым пространством
    centerLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(loginEdit, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addLayout(passwordLayout);
    centerLayout->addSpacing(10);
    centerLayout->addLayout(passwordAgainLayout);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(registerButton, 0, Qt::AlignCenter);
    centerLayout->addStretch(1); //Конец макета с растяжимым пространством
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->setStretchFactor(topLayout, 1); //Меньший вес для topLayout
    mainLayout->addLayout(centerLayout);
    mainLayout->setStretchFactor(centerLayout, 9); //Больший вес для centerLayout
    setLayout(mainLayout);

    connect(backButton, &QPushButton::clicked, this, &RegistrationForm::backButtonClicked);
    connect(registerButton, &QPushButton::clicked, this, &RegistrationForm::registerUser);

}

//Фильтр событий
bool RegistrationForm::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (target == closedEyeLabelPass || target == opendEyeLabelPass)
        {
            bool isPasswordVisible = passwordEdit->echoMode() == QLineEdit::Normal;
            passwordEdit->setEchoMode(isPasswordVisible ? QLineEdit::Password : QLineEdit::Normal);
            closedEyeLabelPass->setVisible(isPasswordVisible); //Если пароль был видим, показываем закрытый глаз
            opendEyeLabelPass->setVisible(!isPasswordVisible); //Если пароль был скрыт, показываем открытый глаз
            return true; //Возвращаем true, чтобы указать, что событие обработано
        }
        else if (target == closedEyeLabelPassAgain || target == opendEyeLabelPassAgain)
        {
            bool isPasswordVisibleAgain = passwordEditAgain->echoMode() == QLineEdit::Normal;
            passwordEditAgain->setEchoMode(isPasswordVisibleAgain ? QLineEdit::Password : QLineEdit::Normal);
            closedEyeLabelPassAgain->setVisible(isPasswordVisibleAgain); //Если пароль был видим, показываем закрытый глаз
            opendEyeLabelPassAgain->setVisible(!isPasswordVisibleAgain); //Если пароль был скрыт, показываем открытый глаз
            return true; //Возвращаем true, чтобы указать, что событие обработано
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
                return true; //Возвращаем true, чтобы указать, что событие обработано
            }
        }
    }
    return QWidget::eventFilter(target, event); //Для всех остальных случаев вызывает базовую реализацию
}

//Обработка нажатия на картинку
void RegistrationForm::onImageLabelClicked()
{
    QMessageBox::information(this, "Информация", "Логин может состоять из символов латиницы любого регистра, цифр, тире и нижнего подчеркивания.\n"
                                                 "Логин должен состоять как минимум из 1 символа, как максимум из 255 символов.\n"
                                                 "Логин должен быть уникальным.\n"
                                                 "Пароль должен состоять из символов латиницы разного регистра, "
                                                 "цифр и специальных символов.\nДлина пароля не может быть менее 8 символов и\n"
                                                 "не может быть более 255 символов.");
}

//Нажатие на кнопку "Назад"
void RegistrationForm::backButtonClicked()
{
    loginEdit->clear();
    passwordEdit->clear();
    passwordEditAgain->clear();
    disconnect(socket, nullptr, this, nullptr);
    emit backRequested();
}

//Нажатие на кнопку "Зарегистрироваться"
void RegistrationForm::registerUser()
{
    QString login = loginEdit->text();
    QString password = passwordEdit->text();
    QString passwordAgain = passwordEditAgain->text();

    if(!login.length() || !password.length() || !passwordAgain.length())
    {
        QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены.");
        return;
    }
    if(!loginContainsOnlyAllowedCharacters(login))
    {
        QMessageBox::warning(this, "Ошибка", "В логине использованы запрещенные символы.");
        return;
    }
    if(password != passwordAgain)
    {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают.");
        return;
    }
    if(password.length() < 8 || !passwordContainsRequiredCharacters(password))
    {
        QMessageBox::warning(this, "Ошибка", "Пароль не соответствует требованиям безопасности.");
        return;
    }

    //Хеширование пароля
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

//Проверка пароля, содержит ли пароль только разрешенные символы и в нужном сочетании
bool RegistrationForm::passwordContainsRequiredCharacters(const QString &password)
{
    QRegularExpression upperCaseRegExp("[A-Z]"); //Регулярное выражение для заглавных букв
    QRegularExpression lowerCaseRegExp("[a-z]"); //Регулярное выражение для строчных букв
    QRegularExpression digitRegExp("\\d");       //Регулярное выражение для цифр
    QRegularExpression specialRegExp("[!@#$%^&*()_+=-]"); //Регулярное выражение для специальных символов

    return password.contains(upperCaseRegExp) &&
           password.contains(lowerCaseRegExp) &&
           password.contains(digitRegExp) &&
           password.contains(specialRegExp);
}

//Проверка логина, содержит ли логин только разрешенные символы
bool RegistrationForm::loginContainsOnlyAllowedCharacters(const QString &login)
{
    QRegularExpression loginRegExp("^[A-Za-z\\d_-]+$"); //Регулярное выражение для допустимых символов в логине
    return login.contains(loginRegExp);
}

//Обработка ответов от сервера
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
        emit backRequested();  // Предположим, что этот сигнал заставит MainWindow показать LoginForm
    }
    else
    {
        QMessageBox::critical(this, tr("Ошибка регистрации"), tr("Данный логин уже используется. Придумайте другой."));
        loginEdit->clear();
        passwordEdit->clear();
        passwordEditAgain->clear();
    }
}

//Подключение к сокету для отправки сообщений на сервер и получения ответов
void RegistrationForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &RegistrationForm::handleServerResponse);
}
