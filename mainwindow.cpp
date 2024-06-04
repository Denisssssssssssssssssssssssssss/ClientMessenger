#include "mainwindow.h"

#include "LoginForm.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    resize(window_width, window_height);

    loginForm = new LoginForm(this);
    registrationForm = new RegistrationForm(this);

    setCentralWidget(loginForm);

    connect(loginForm, &LoginForm::registerRequested, this, &MainWindow::showRegistrationForm);

    setWindowTitle(tr("Вход"));
}

//Слот для отображения формы авторизации
void MainWindow::showLoginForm()
{
    setCentralWidget(loginForm);
    loginForm->show();
}

//Слот для отображения формы регистрации
void MainWindow::showRegistrationForm()
{
    setCentralWidget(registrationForm);
    registrationForm->show();
}
