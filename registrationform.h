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
    Q_OBJECT

private:
    QLabel *registerLabel;
    QLabel *titleLabel;
    QLabel* imageLabel; //Картинка открывающая информацию
    QLabel* opendEyeLabelPass; //Картинка открытого глаза
    QLabel* closedEyeLabelPass; //Картинка закрытого глаза
    QLabel* opendEyeLabelPassAgain;
    QLabel* closedEyeLabelPassAgain;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *passwordEditAgain;
    QPushButton *registerButton;
    QPushButton *backButton;

    bool eventFilter(QObject *target, QEvent *event);

private slots:
    void onImageLabelClicked();
    void backButtonClicked();

public:
    explicit RegistrationForm(QWidget *parent = nullptr);

signals:
    void backRequested();

};

#endif // REGISTRATIONFORM_H
