#include "mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/images/logo.png"));
    resize(window_width, window_height);
    loginForm = new LoginForm(this);
    registrationForm = new RegistrationForm(this);
    setCentralWidget(loginForm);
    connect(loginForm, &LoginForm::registerRequested, this, &MainWindow::showRegistrationForm);
    connect(registrationForm, &RegistrationForm::backRequested, this, &MainWindow::showLoginForm);
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
