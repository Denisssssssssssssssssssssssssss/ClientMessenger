#include "LoginForm.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QMessageBox>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>

LoginForm::LoginForm(QTcpSocket *socket, QWidget *parent) : QWidget(parent), socket(socket)
{
    registerLabel = new QLabel(tr("Еще нет аккаунта? <a href=\"register\" style=\"color:#1E90FF;\">Зарегистрироваться</a>"));
    QFont titleFont = registerLabel->font();
    titleFont.setPointSize(10);
    registerLabel->setFont(titleFont);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(registerLabel, 0, Qt::AlignTop | Qt::AlignRight);

    titleLabel = new QLabel(tr("Вход"));
    titleFont.setPointSize(14);
    titleLabel->setFont(titleFont);

    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText(tr("Введите логин..."));
    loginEdit->setFixedWidth(200);

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText(tr("Введите пароль..."));
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedWidth(200);

    //Инициализация иконок глаза
    openedEyeLabelPass = new QLabel(this);
    closedEyeLabelPass = new QLabel(this);
    QPixmap openedEyePixmap(":/images/icon-eye-opened.png");
    QPixmap closedEyePixmap(":/images/icon-eye-closed.png");
    closedEyeLabelPass->setPixmap(closedEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    openedEyeLabelPass->setPixmap(openedEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    openedEyeLabelPass->hide();

    //Горизонтальный лэйаут для поля ввода пароля и иконок
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    passwordLayout->addStretch(5);
    passwordLayout->addWidget(passwordEdit);
    passwordLayout->addWidget(openedEyeLabelPass);
    passwordLayout->addWidget(closedEyeLabelPass);
    passwordLayout->addStretch(4);

    openedEyeLabelPass->installEventFilter(this);
    closedEyeLabelPass->installEventFilter(this);

    loginButton = new QPushButton(tr("Войти"));
    loginButton->setFixedWidth(120);

    QVBoxLayout *centerLayout = new QVBoxLayout();
    centerLayout->addStretch(1);
    centerLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(loginEdit, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addLayout(passwordLayout);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(loginButton, 0, Qt::AlignCenter);
    centerLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(centerLayout);
    setLayout(mainLayout);

    connect(registerLabel, &QLabel::linkActivated, this, &LoginForm::onRegisterClicked);
    connect(loginButton, &QPushButton::clicked, this, &LoginForm::attemptLogin);

}

//Фильтр событий
bool LoginForm::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (obj == closedEyeLabelPass || obj == openedEyeLabelPass)
        {
            //Переключение видимости пароля для поля passwordEdit
            bool isPasswordVisible = passwordEdit->echoMode() == QLineEdit::Normal;
            passwordEdit->setEchoMode(isPasswordVisible ? QLineEdit::Password : QLineEdit::Normal);
            closedEyeLabelPass->setVisible(isPasswordVisible); //Если пароль был видим, показываем закрытый глаз
            openedEyeLabelPass->setVisible(!isPasswordVisible); //Если пароль был скрыт, показываем открытый глаз
            return true; //Возвращаем true, чтобы указать, что событие обработано
        }
    }
    return QWidget::eventFilter(obj, event); //Вызов базового обработчика для других классов
}

//Нажатие на ссылку "Зарегистрироваться"
void LoginForm::onRegisterClicked()
{
    //Используется для эмитирования сигнала при нажатии на ссылку регистрации
    loginEdit->clear();
    passwordEdit->clear();
    disconnect(socket, nullptr, this, nullptr);
    emit registerRequested();
}

//Нажатие на кнопку "Войти"
void LoginForm::attemptLogin()
{
    QString login = loginEdit->text();
    QString password = passwordEdit->text();

    if(login.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, tr("Ошибка входа"), tr("Все поля должны быть заполнены."));
        return;
    }

    //Хеширование пароля
    QByteArray byteArrayPasswordSalt = (password + login).toUtf8();
    QByteArray hashedPassword = QCryptographicHash::hash(byteArrayPasswordSalt, QCryptographicHash::Sha256).toHex();

    QJsonObject loginRequest;
    loginRequest["type"] = "login";
    loginRequest["login"] = login;
    loginRequest["password"] = QString(hashedPassword);

    QByteArray dataArray = QJsonDocument(loginRequest).toJson(QJsonDocument::Compact);

    socket->write(dataArray);
    socket->flush();
}

//Обработка ответа от сервера
void LoginForm::handleServerResponse()
{
    QByteArray responseData = socket->readAll();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if(jsonObj["status"].toString() == "success")
    {
        login = loginEdit->text();
        qDebug() << login << "\n";
        disconnect(socket, nullptr, this, nullptr);
        loginEdit->clear();
        passwordEdit->clear();
        emit loginSuccess();
    }
    else
    {
        loginEdit->clear();
        passwordEdit->clear();
        QMessageBox::critical(this, tr("Ошибка входа"), tr("Введен неправильно логин или пароль. Попробуйте еще раз"));
    }
}

QString LoginForm::getLogin() { return login; }

//Подключение к сокету для отправки сообщений на сервер и получения ответов
void LoginForm::connectSocket()
{
    connect(socket, &QTcpSocket::readyRead, this, &LoginForm::handleServerResponse);
}
