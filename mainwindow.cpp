/**
 * /file mainwindow.cpp
 * /brief Реализация главного окна мессенджера.
 */

#include "mainwindow.h"

#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

/**
 * /brief Конструктор MainWindow.
 * /param socket Указатель на QTcpSocket для работы с сетевыми соединениями.
 * /param parent Указатель на родительский виджет.
 */
MainWindow::MainWindow(QTcpSocket *socket, QWidget *parent) : QMainWindow(parent), socket(socket)
{
    setWindowIcon(QIcon(":/images/logo.png"));
    resize(window_width, window_height);
    loginForm = new LoginForm(socket, this);
    registrationForm = new RegistrationForm(socket, this);

    setCentralWidget(loginForm);

    connect(loginForm, &LoginForm::registerRequested, this, &MainWindow::showRegistrationForm);
    connect(registrationForm, &RegistrationForm::backRequested, this, &MainWindow::showLoginForm);

    // Если хотим иметь возможность возвращаться на виджет LoginForm после неудачной регистрации
    connect(registrationForm, &RegistrationForm::backRequested, this, &MainWindow::loginRequested);

    connect(loginForm, &LoginForm::loginSuccess, this, &MainWindow::onLoginSuccess);

    setWindowTitle(tr("Вход"));
    loginForm->connectSocket();
}

/**
 * /brief Слот для отображения формы авторизации.
 */
void MainWindow::showLoginForm()
{
    disconnect(socket, nullptr, this, nullptr);
    QWidget *currentCentralWidget = centralWidget();
    if(currentCentralWidget == registrationForm)
    {
        if (centralWidget()->layout())
        {
            centralWidget()->layout()->removeWidget(registrationForm); // Удаляет виджет из лейаута
            registrationForm->setParent(nullptr); // Отсоединить виджет, чтобы избежать его удаления
        }
    }
    if(currentCentralWidget == messengerForm)
    {
        if (centralWidget()->layout())
        {
            centralWidget()->layout()->removeWidget(messengerForm); // Удаляет виджет из лейаута
            messengerForm->setParent(nullptr); // Отсоединить виджет, чтобы избежать его удаления
        }
    }
    setCentralWidget(loginForm);
    if(currentCentralWidget == messengerForm)
        loginForm->show();
    setWindowTitle(tr("Вход"));
    loginForm->connectSocket();
}

/**
 * /brief Слот для отображения формы регистрации.
 */
void MainWindow::showRegistrationForm()
{
    disconnect(socket, nullptr, this, nullptr);
    if (centralWidget()->layout())
    {
        centralWidget()->layout()->removeWidget(loginForm); // Удаляет виджет из лейаута
    }
    loginForm->setParent(nullptr); // Отсоединить виджет, чтобы избежать его удаления
    setCentralWidget(registrationForm);
    setWindowTitle(tr("Регистрация"));
    registrationForm->connectSocket();
}

/**
 * /brief Слот для отображения формы ввода имени.
 */
void MainWindow::showNicknameForm()
{
    disconnect(socket, nullptr, this, nullptr);

    if (centralWidget()->layout())
    {
        centralWidget()->layout()->removeWidget(loginForm); // Удаляет виджет из лейаута
    }
    loginForm->setParent(nullptr);
    setCentralWidget(nicknameForm);
    setWindowTitle(tr("Обновление имени"));
    connect(nicknameForm, &NicknameForm::nicknameUpdated, this, &MainWindow::showMessengerForm);
    nicknameForm->connectSocket();
}

/**
 * /brief Слот для отображения формы списка чатов.
 */
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
    else if (currentCentralWidget == groupchatForm)
    {
        centralWidget()->layout()->removeWidget(groupchatForm);
        groupchatForm->setParent(nullptr);
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

/**
 * /brief Слот для отображения формы настроек.
 */
void MainWindow::showSettingsForm()
{
    disconnect(socket, nullptr, this, nullptr);
    if (centralWidget()->layout())
    {
        centralWidget()->layout()->removeWidget(messengerForm); // Удаляет виджет из лейаута
    }
    messengerForm->setParent(nullptr); // Отсоединить виджет, чтобы избежать его удаления
    setCentralWidget(settingsForm);
    setWindowTitle(tr("Настройки"));
    settingsForm->connectSocket();
    settingsForm->requestNickname();
}

/**
 * /brief Обработка закрытия окна.
 * /param event Указатель на событие закрытия.
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, tr("Подтверждение"),
                                                               tr("Закрыть мессенджер?"), QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

/**
 * /brief Слот, вызываемый при успешной авторизации.
 */
void MainWindow::onLoginSuccess()
{
    loginForm->hide();
    login = loginForm->getLogin();
    qDebug() << login << "\n";
    settingsForm = new SettingsForm(socket, login, this);
    messengerForm = new MessengerForm(socket, login, this);
    nicknameForm = new NicknameForm(socket, login, this);
    connect(messengerForm, &MessengerForm::settingsRequested, this, &MainWindow::showSettingsForm);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::receiveNicknameStatus);
    connect(messengerForm, &MessengerForm::logoutRequested, this, &MainWindow::handleLogout);
    connect(settingsForm, &SettingsForm::backToMessengerFormRequested, this, &MainWindow::showMessengerForm);
    connect(messengerForm, &MessengerForm::chatRequested, this, &MainWindow::onChatRequested);
    isNicknameNewUser();
}

/**
 * /brief Запрос на открытие чата.
 * /param chatId Идентификатор чата.
 * /param userNickname Псевдоним пользователя.
 * /param chatType Тип чата ("personal" или "group").
 */
void MainWindow::onChatRequested(QString chatId, QString userNickname, QString chatType)
{
    qDebug() << "MainWindow received chatRequested signal with chatId:" << chatId << " and userNickname:" << userNickname;
    showChatForm(chatId, userNickname, chatType);
}

/**
 * /brief Слот для отображения формы чата.
 * /param chatId Идентификатор чата.
 * /param userNickname Псевдоним пользователя.
 * /param chatType Тип чата ("personal" или "group").
 */
void MainWindow::showChatForm(QString chatId, QString userNickname, QString chatType)
{
    disconnect(socket, nullptr, this, nullptr);  // Отключаем все предыдущие подключения к сокетам
    if (centralWidget()->layout()) {
        centralWidget()->layout()->removeWidget(messengerForm);
        messengerForm->setParent(nullptr);
    }

    if (chatType == "personal") {
        // Если тип чата персональный, создаем ChatForm
        chatForm = new ChatForm(socket, login, chatId, this);
        connect(chatForm, &ChatForm::backRequested, this, &MainWindow::showMessengerForm);  // Подключаем слот для возврата
        setCentralWidget(chatForm);
        setWindowTitle(tr("Чат с ") + userNickname);
    } else if (chatType == "group") {
        // Если тип чата групповой, создаем GroupChatForm
        groupchatForm = new GroupChatForm(socket, login, chatId, this);
        connect(groupchatForm, &GroupChatForm::openSettingsRequested, this, &MainWindow::showSettingsGroupChatForm);
        setCentralWidget(groupchatForm);
        setWindowTitle(tr("Групповой чат: ") + userNickname);
    }

    // Устанавливаем соединение с сокетом для нового чата
    if (chatType == "personal") {
        chatForm->connectSocket();
    } else if (chatType == "group") {
        groupchatForm->connectSocket();
    }
}

/**
 * /brief Слот для отображения формы группового чата.
 * /param chatId Идентификатор чата.
 * /param userNickname Псевдоним пользователя.
 */
void MainWindow::showGroupChatForm(QString chatId, QString userNickname)
{
    disconnect(socket, nullptr, this, nullptr);  // Отключаем все предыдущие подключения к сокетам
    QWidget *currentCentralWidget = centralWidget();
    if(currentCentralWidget == settingsgroupchatForm)
    {
        if (centralWidget()->layout())
        {
            centralWidget()->layout()->removeWidget(settingsgroupchatForm); // Удаляет виджет из лейаута
            settingsgroupchatForm->setParent(nullptr); // Отсоединить виджет, чтобы избежать его удаления
        }
    }
    if(currentCentralWidget == messengerForm)
    {
        if (centralWidget()->layout())
        {
            centralWidget()->layout()->removeWidget(messengerForm); // Удаляет виджет из лейаута
            messengerForm->setParent(nullptr); // Отсоединить виджет, чтобы избежать его удаления
        }
    }

    groupchatForm = new GroupChatForm(socket, login, chatId, this);
    connect(groupchatForm, &GroupChatForm::backToMessengerFormRequested, this, &MainWindow::showMessengerForm);  // Подключаем слот для возврата
    connect(groupchatForm, &GroupChatForm::sendChatId, this, &MainWindow::setChatId);
    setCentralWidget(groupchatForm);
    setWindowTitle(tr("Чат с ") + userNickname);
    groupchatForm->connectSocket();
}

/**
 * /brief Устанавливает идентификатор чата.
 * /param chatId Идентификатор чата.
 */
void MainWindow::setChatId(QString chatId)
{
    this->chatId = chatId;
}

/**
 * /brief Проверяет, является ли имя пользователя новым.
 */
void MainWindow::isNicknameNewUser()
{
    QJsonObject request;
    request["type"] = "check_nickname";
    request["login"] = login;

    QByteArray requestData = QJsonDocument(request).toJson();
    socket->write(requestData);
    socket->flush();
}

/**
 * /brief Обрабатывает статус ника пользователя.
 */
void MainWindow::receiveNicknameStatus()
{
    // Имеются ли доступные данные для чтения
    if (socket->bytesAvailable() == 0)
    {
        return;
    }

    QByteArray rawData = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    qDebug() << QJsonDocument(doc).toJson(QJsonDocument::Compact);
    if (!doc.isObject())
    {
        return; // Если данные не могут быть разобраны как JSON объект, выходим из метода
    }
    QJsonObject response = doc.object();
    qDebug() << "MainWindow Response data:" << response;

    if (response.contains("nickname") && response["nickname"].toString() == "New user")
    {
        qDebug() << "New user\n";
        qDebug() << response["nickname"].toString();
        // Передать подключение
        disconnect(socket, nullptr, this, nullptr);
        showNicknameForm();
    } else {
        qDebug() << "Not new user\n";
        qDebug() << response["nickname"].toString();
        // Передать подключение
        disconnect(socket, nullptr, this, nullptr);
        showMessengerForm();
    }
}

/**
 * /brief Слот для отображения формы настроек группового чата.
 */
void MainWindow::showSettingsGroupChatForm()
{
    settingsgroupchatForm = new SettingsGroupChatForm(socket, login, chatId, this);
    connect(settingsgroupchatForm, &SettingsGroupChatForm::backToMessengerFormRequested, this, &MainWindow::showMessengerForm);
    QWidget *currentCentralWidget = centralWidget();
    if (centralWidget()->layout())
    {
        centralWidget()->layout()->removeWidget(groupchatForm); // Удаляет виджет из лейаута
        groupchatForm->setParent(nullptr); // Отсоединить виджет, чтобы избежать его удаления
    }
    setCentralWidget(settingsgroupchatForm);
    if(currentCentralWidget == groupchatForm)
        settingsgroupchatForm->show();
    setWindowTitle(tr("Настройки группового чата"));
    settingsgroupchatForm->connectSocket();
}

/**
 * /brief Обрабатывает выход из аккаунта.
 */
void MainWindow::handleLogout() { showLoginForm(); }
