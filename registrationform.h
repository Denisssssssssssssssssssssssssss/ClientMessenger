#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QEvent>
#include <QMouseEvent>

class RegistrationForm : public QWidget
{
private:
    QLabel *registerLabel;
    QLabel *titleLabel;
    QLabel* imageLabel;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *passwordEditAgain;
    QPushButton *registerButton;
    QPushButton *backButton;

    bool eventFilter(QObject *target, QEvent *event);

private slots:
    void onImageLabelClicked();

public:
    explicit RegistrationForm(QWidget *parent = nullptr);

};

#endif // REGISTRATIONFORM_H
