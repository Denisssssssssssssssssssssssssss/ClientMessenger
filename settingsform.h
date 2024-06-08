#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class SettingsForm : public QWidget
{
    Q_OBJECT

private:
    QTcpSocket *socket;
    QString login;
    QPushButton *backButton;
    QPushButton *blacklistButton;
    QPushButton *changeLoginButton;
    QPushButton *changeNameButton;
    QPushButton *changePasswordButton;
    QLabel *loginLabel;
    QLabel *nameLabel;
    QLineEdit *loginEdit;
    QLineEdit *nameEdit;

public:
    explicit SettingsForm(QTcpSocket *socket, QString login, QWidget *parent = nullptr);

signals:

};

#endif // SETTINGSFORM_H
