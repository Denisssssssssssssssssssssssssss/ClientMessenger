#ifndef MESSENGERFORM_H
#define MESSENGERFORM_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MessengerForm : public QWidget
{
    Q_OBJECT

public:
    explicit MessengerForm(QWidget *parent = nullptr);

private slots:
    void openSettings();
    void findChats();
    void logOut();

private:
    QPushButton *settingsButton;
    QPushButton *findButton;
    QLineEdit *searchEdit;
    QListWidget *chatList;
    QPushButton *logOutButton;
};

#endif // MESSENGERFORM_H
