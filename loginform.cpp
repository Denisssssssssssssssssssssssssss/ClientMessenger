#include "LoginForm.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QCoreApplication>

LoginForm::LoginForm(QTcpSocket *socket, QWidget *parent) : QWidget(parent), socket(socket)
{
    registerLabel = new QLabel(tr("Еще нет аккаунта? <a href=\"register\" style=\"color:#1E90FF;\">Зарегистрироваться</a>"));
    QFont titleFont = registerLabel->font();
    titleFont.setPointSize(10);
    registerLabel->setFont(titleFont);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(registerLabel, 0, Qt::AlignTop | Qt::AlignRight);

    titleLabel = new QLabel(tr("Вход"));
    titleFont.setPointSize(14);
    titleLabel->setFont(titleFont);

    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText(tr("Введите логин..."));
    loginEdit->setFixedWidth(200);

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText(tr("Введите пароль..."));
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedWidth(200);

    // Инициализация иконок глаза
    openedEyeLabelPass = new QLabel(this);
    closedEyeLabelPass = new QLabel(this);
    QPixmap openedEyePixmap(":/images/icon-eye-opened.png");
    QPixmap closedEyePixmap(":/images/icon-eye-closed.png");
    closedEyeLabelPass->setPixmap(closedEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    openedEyeLabelPass->setPixmap(openedEyePixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    openedEyeLabelPass->hide();

    // Horizontal layout for password field and toggle visibility icons
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    passwordLayout->addStretch(5);
    passwordLayout->addWidget(passwordEdit);
    passwordLayout->addWidget(openedEyeLabelPass);
    passwordLayout->addWidget(closedEyeLabelPass);
    passwordLayout->addStretch(4);

    openedEyeLabelPass->installEventFilter(this);
    closedEyeLabelPass->installEventFilter(this);

    loginButton = new QPushButton(tr("Войти"));
    loginButton->setFixedWidth(120);

    QVBoxLayout *centerLayout = new QVBoxLayout();
    centerLayout->addStretch(1);
    centerLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(loginEdit, 0, Qt::AlignCenter);
    centerLayout->addSpacing(10);
    centerLayout->addLayout(passwordLayout); // Используем passwordLayout здесь
    centerLayout->addSpacing(10);
    centerLayout->addWidget(loginButton, 0, Qt::AlignCenter);
    centerLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(centerLayout);
    setLayout(mainLayout);

    connect(registerLabel, &QLabel::linkActivated, this, &LoginForm::onRegisterClicked);
}

bool LoginForm::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (obj == closedEyeLabelPass || obj == openedEyeLabelPass)
        {
            //Переключение видимости пароля для поля passwordEdit
            bool isPasswordVisible = passwordEdit->echoMode() == QLineEdit::Normal;
            passwordEdit->setEchoMode(isPasswordVisible ? QLineEdit::Password : QLineEdit::Normal);
            closedEyeLabelPass->setVisible(isPasswordVisible); // Если пароль был видим, показываем закрытый глаз
            openedEyeLabelPass->setVisible(!isPasswordVisible); // Если пароль был скрыт, показываем открытый глаз
            return true; // Возвращаем true, чтобы указать, что событие обработано
        }
    }
    return QWidget::eventFilter(obj, event); // Call base implementation for other cases
}


void LoginForm::onRegisterClicked()
{
    // Используется для эмитирования сигнала при нажатии на ссылку регистрации
    loginEdit->clear();
    passwordEdit->clear();
    emit registerRequested();
}
