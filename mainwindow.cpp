#include "mainwindow.h"

#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QTcpSocket *socket, QWidget *parent) : QMainWindow(parent), socket(socket)
{
    setWindowIcon(QIcon(":/images/logo.png"));
    resize(window_width, window_height);
    loginForm = new LoginForm(socket, this);
    registrationForm = new RegistrationForm(socket, this);

    setCentralWidget(loginForm);

    connect(loginForm, &LoginForm::registerRequested, this, &MainWindow::showRegistrationForm);
    connect(registrationForm, &RegistrationForm::backRequested, this, &MainWindow::showLoginForm);

    // Если мы хотим иметь возможность возвращаться на виджет LoginForm после неудачной регистрации
    connect(registrationForm, &RegistrationForm::backRequested, this, &MainWindow::loginRequested);

    connect(loginForm, &LoginForm::loginSuccess, this, &MainWindow::onLoginSuccess);

    setWindowTitle(tr("Вход"));
    loginForm->connectSocket();
}

//Слот для отображения формы авторизации
void MainWindow::showLoginForm()
{
    disconnect(socket, nullptr, this, nullptr);
    QWidget *currentCentralWidget = centralWidget();
    if(currentCentralWidget == registrationForm)
    {
        if (centralWidget()->layout())
        {
            centralWidget()->layout()->removeWidget(registrationForm); //Удаляет виджет из лейаута
             registrationForm->setParent(nullptr); //Отсоединить виджет, чтобы избежать его удаления
        }
    }
    if(currentCentralWidget == messengerForm)
    {
        if (centralWidget()->layout())
        {
            centralWidget()->layout()->removeWidget(messengerForm); //Удаляет виджет из лейаута
            messengerForm->setParent(nullptr); //Отсоединить виджет, чтобы избежать его удаления
        }
    }
    setCentralWidget(loginForm); //Теперь устанавливаем loginForm как центральный виджет
    if(currentCentralWidget == messengerForm)
        loginForm->show();
    setWindowTitle(tr("Вход"));
    loginForm->connectSocket();
}

//Слот для отображения формы регистрации
void MainWindow::showRegistrationForm()
{
    disconnect(socket, nullptr, this, nullptr);
    if (centralWidget()->layout())
    {
        centralWidget()->layout()->removeWidget(loginForm); //Удаляет виджет из лейаута
    }
    loginForm->setParent(nullptr); //Отсоединить виджет, чтобы избежать его удаления
    setCentralWidget(registrationForm); //Теперь установаем registrationForm как центральный виджет
    setWindowTitle(tr("Регистрация"));
    registrationForm->connectSocket();
}

void MainWindow::showNicknameForm()
{
    disconnect(socket, nullptr, this, nullptr);

    if (centralWidget()->layout())
    {
        centralWidget()->layout()->removeWidget(loginForm); //Удаляет виджет из лейаута
    }
    loginForm->setParent(nullptr);
    setCentralWidget(nicknameForm);
    setWindowTitle(tr("Обновление имени"));
    connect(nicknameForm, &NicknameForm::nicknameUpdated, this, &MainWindow::showMessengerForm);
    nicknameForm->connectSocket();
}

void MainWindow::showMessengerForm()
{
    disconnect(socket, nullptr, this, nullptr);
    QWidget *currentCentralWidget = centralWidget();
    if (currentCentralWidget == loginForm)
    {
        centralWidget()->layout()->removeWidget(loginForm);
        loginForm->setParent(nullptr);
    }
    else if (currentCentralWidget == nicknameForm)
    {
        centralWidget()->layout()->removeWidget(nicknameForm);
        nicknameForm->setParent(nullptr);
    }
    else if (currentCentralWidget == settingsForm)
    {
        centralWidget()->layout()->removeWidget(settingsForm);
        settingsForm->setParent(nullptr);
    }
    else if (currentCentralWidget == chatForm)
    {
        centralWidget()->layout()->removeWidget(chatForm);
        chatForm->setParent(nullptr);
    }

    setCentralWidget(messengerForm);
    setWindowTitle(tr("Мессенджер"));
    messengerForm->connectSocket();
    messengerForm->requestChatList();
}

void MainWindow::showSettingsForm()
{
    disconnect(socket, nullptr, this, nullptr);
    if (centralWidget()->layout())
    {
        centralWidget()->layout()->removeWidget(messengerForm); //Удаляет виджет из лейаута
    }
    messengerForm->setParent(nullptr); //Отсоединить виджет, чтобы избежать его удаления
    setCentralWidget(settingsForm);
    setWindowTitle(tr("Настройки"));
    settingsForm->connectSocket();
    settingsForm->requestNickname();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, tr("Подтверждение"),
                                                               tr("Вы уверены, что хотите закрыть мессенджер?"), QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::onLoginSuccess() {
    loginForm->hide(); // Скрыть LoginForm
    login = loginForm->getLogin(); // Предположим, что у вас есть метод получения логина
    qDebug() << login << "\n";
    settingsForm = new SettingsForm(socket, login, this);
    messengerForm = new MessengerForm(socket, login, this);
    nicknameForm = new NicknameForm(socket, login, this);
    connect(messengerForm, &MessengerForm::settingsRequested, this, &MainWindow::showSettingsForm);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::receiveNicknameStatus);
    connect(messengerForm, &MessengerForm::logoutRequested, this, &MainWindow::handleLogout);
    connect(settingsForm, &SettingsForm::backToMessengerFormRequested, this, &MainWindow::showMessengerForm);
    connect(messengerForm, &MessengerForm::chatRequested, this, &MainWindow::showChatForm);
    isNicknameNewUser();
}

void MainWindow::isNicknameNewUser() {
    // Создание JSON запроса на получение статуса никнейма
    QJsonObject request;
    request["type"] = "check_nickname";
    request["login"] = login;

    // Отправка запроса на сервер
    QByteArray requestData = QJsonDocument(request).toJson();
    socket->write(requestData);
    socket->flush();
}

void MainWindow::receiveNicknameStatus() {
    // Проверка, имеются ли доступные данные для чтения
    if (socket->bytesAvailable() == 0) {
        return; // Если данных нет, просто выходим из метода
    }

    QByteArray rawData = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    qDebug() << QJsonDocument(doc).toJson(QJsonDocument::Compact);
    if (!doc.isObject()) {
        return; // Если данные не могут быть разобраны как JSON объект, выходим из метода
    }
    QJsonObject response = doc.object();
    qDebug() << "MessengerForm Response data:" << response;
    // Обработка ответа от сервера
    if (response.contains("nickname") && response["nickname"].toString() == "New user") {
        qDebug() << "New user\n";
        qDebug() << response["nickname"].toString();

        //передать подключение
        disconnect(socket, nullptr, this, nullptr);
        showNicknameForm();
    } else {
        qDebug() << "Not new user\n";
        qDebug() << response["nickname"].toString();
        //передать подключение
        disconnect(socket, nullptr, this, nullptr);
        showMessengerForm();
    }
}

void MainWindow::handleLogout()
{
    showLoginForm();
}

void MainWindow::showChatForm(QString user)
{
    disconnect(socket, nullptr, this, nullptr);
    centralWidget()->layout()->removeWidget(messengerForm);
    messengerForm->setParent(nullptr);

    chatForm = new ChatForm(socket, login, this); // Создаем новый объект ChatForm
    connect(chatForm, &ChatForm::backRequested, this, &MainWindow::showMessengerForm); // Подключаем слот для возврата в MessengerForm

    setCentralWidget(chatForm);
    setWindowTitle(tr("Чат с ") + user);
    chatForm->connectSocket(); // Предположим, что метод connectSocket() добавлен в ChatForm
}
