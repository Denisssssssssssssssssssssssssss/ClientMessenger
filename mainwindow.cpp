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

    nicknameForm = new NicknameForm(this);
    messengerForm = new MessengerForm(this);

    setCentralWidget(loginForm);
    connect(loginForm, &LoginForm::registerRequested, this, &MainWindow::showRegistrationForm);
    connect(registrationForm, &RegistrationForm::backRequested, this, &MainWindow::showLoginForm);

    // Если мы хотим иметь возможность возвращаться на виджет LoginForm после неудачной регистрации
    connect(registrationForm, &RegistrationForm::backRequested, this, &MainWindow::loginRequested);

    connect(loginForm, &LoginForm::loginSuccess, this, &MainWindow::onLoginSuccess);

    //connect(this, &MainWindow::checkNicknameStatus, this, &MainWindow::isNicknameNewUser);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::receiveNicknameStatus);

    setWindowTitle(tr("Вход"));
}

//Слот для отображения формы авторизации
void MainWindow::showLoginForm()
{
    if (centralWidget()->layout())
    {
        centralWidget()->layout()->removeWidget(registrationForm); //Удаляет виджет из лейаута
    }
    registrationForm->setParent(nullptr); //Отсоединить виджет, чтобы избежать его удаления
    setCentralWidget(loginForm); //Теперь устанавливаем loginForm как центральный виджет
    setWindowTitle(tr("Вход"));
}

//Слот для отображения формы регистрации
void MainWindow::showRegistrationForm()
{
    if (centralWidget()->layout())
    {
        centralWidget()->layout()->removeWidget(loginForm); //Удаляет виджет из лейаута
    }
    loginForm->setParent(nullptr); //Отсоединить виджет, чтобы избежать его удаления
    setCentralWidget(registrationForm); //Теперь установаем registrationForm как центральный виджет
    setWindowTitle(tr("Регистрация"));
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
    if (!doc.isObject()) {
        return; // Если данные не могут быть разобраны как JSON объект, выходим из метода
    }
    QJsonObject response = doc.object();

    // Обработка ответа от сервера
    if (response.contains("nickname") && response["nickname"].toString() == "New user") {
        qDebug() << "New user\n";
        qDebug() << response["nickname"].toString();
        showNicknameForm();
    } else {
        qDebug() << "Not new user\n";
        qDebug() << response["nickname"].toString();
        showMessengerForm();
    }
}


void MainWindow::showNicknameForm() {
    // Удаляем текущий центральный виджет, если таковой имеется
    QWidget *currentWidget = centralWidget();
    setCentralWidget(nullptr);
    if (currentWidget && currentWidget != nicknameForm) {
        currentWidget->setParent(nullptr); // Отсекаем текущий виджет от MainWindow
        delete currentWidget; // Важно очищать память
    }

    // Если nicknameForm еще не создан, создаем новый экземпляр
    if (!nicknameForm) {
        nicknameForm = new NicknameForm(this);
        // Здесь можно подключить сигналы от nicknameForm к нужным слотам
    }

    // Устанавливаем nicknameForm как центральный виджет
    setCentralWidget(nicknameForm);

    // Можно добавить обновление заголовка окна при необходимости
    setWindowTitle(tr("Обновление никнейма"));
}

void MainWindow::showMessengerForm() {
    // Удаляем текущий центральный виджет, если таковой имеется
    QWidget *currentWidget = centralWidget();
    setCentralWidget(nullptr);
    if (currentWidget && currentWidget != messengerForm) {
        currentWidget->setParent(nullptr); // Отсекаем текущий виджет от MainWindow
        delete currentWidget; // Важно очищать память
    }

    // Если messengerForm еще не создан, создаем новый экземпляр
    if (!messengerForm) {
        messengerForm = new MessengerForm(this);
        // Здесь можно подключить сигналы от messengerForm к нужным слотам
    }

    // Устанавливаем messengerForm как центральный виджет
    setCentralWidget(messengerForm);

    // Можно добавить обновление заголовка окна при необходимости
    setWindowTitle(tr("Мессенджер"));
}
