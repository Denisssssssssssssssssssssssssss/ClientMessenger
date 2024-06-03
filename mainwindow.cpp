#include "mainwindow.h"

#include "LoginForm.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    resize(window_width, window_height);
    LoginForm *loginForm = new LoginForm(this);
    setCentralWidget(loginForm);

    // Можно соединить сигналы и слоты, если это необходимо
    //connect(loginForm, &LoginForm::registerRequested, this, &MainWindow::showRegistrationForm);

    setWindowTitle(tr("Вход"));
}

