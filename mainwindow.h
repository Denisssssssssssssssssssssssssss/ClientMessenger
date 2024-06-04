#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "loginform.h"
#include "registrationform.h"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    LoginForm *loginForm;
    RegistrationForm *registrationForm;

    unsigned int window_width = 500;
    unsigned int window_height = 500;

private slots:
    void showLoginForm();
    void showRegistrationForm();

protected:
    void closeEvent(QCloseEvent *event) override;

public:
    MainWindow(QWidget *parent = nullptr);

};
#endif // MAINWINDOW_H
