#ifndef NICKNAMEFORM_H
#define NICKNAMEFORM_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>
#include <QTcpSocket>

class NicknameForm : public QWidget
{
    Q_OBJECT

private:
    QLabel *instructionsLabel;
    QLineEdit *nicknameEdit;
    QPushButton *saveButton;
    QTcpSocket *socket;
    QString login;

private slots:
    void saveNickname();
    void handleServerResponse();

public:
    explicit NicknameForm(QTcpSocket *socket, QString login, QWidget *parent = nullptr);
    void connectSocket();

signals:
    void nicknameUpdated(); //Сигнал об успешном обновлении никнейма

};

#endif // NICKNAMEFORM_H
