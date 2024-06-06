#ifndef NICKNAMEFORM_H
#define NICKNAMEFORM_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>

class NicknameForm : public QWidget
{
    Q_OBJECT

public:
    explicit NicknameForm(QWidget *parent = nullptr);

private slots:
    void saveNickname();

private:
    QLabel *instructionsLabel;
    QLineEdit *nicknameEdit;
    QPushButton *saveButton;
};

#endif // NICKNAMEFORM_H
