#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QTcpSocket>

/*!
 * \brief Класс формы входа в систему.
 *
 * Этот класс реализует пользовательский интерфейс для входа в систему,
 * включая поля для ввода логина и пароля, а также кнопки для входа и регистрации.
 */
class LoginForm : public QWidget
{
    Q_OBJECT

private:
    QLabel *registerLabel;              /*!< Метка для регистрации */
    QLabel *titleLabel;                 /*!< Заголовок формы */
    QLabel *openedEyeLabelPass;         /*!< Картинка открытого глаза */
    QLabel *closedEyeLabelPass;         /*!< Картинка закрытого глаза */
    QLineEdit *loginEdit;               /*!< Поле ввода логина */
    QLineEdit *passwordEdit;            /*!< Поле ввода пароля */
    QPushButton *loginButton;           /*!< Кнопка для входа */
    QTcpSocket *socket;                 /*!< Сокет для сетевого взаимодействия */
    QString login;                      /*!< Логин пользователя */

    /*!
     * \brief Фильтр событий.
     *
     * Обрабатывает события, происходящие в виджетах.
     *
     * \param obj Указатель на объект, который отправил событие.
     * \param event Указатель на объект события.
     * \return true, если событие было обработано, иначе false.
     */
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    /*!
     * \brief Обработчик клика на ссылку "Зарегистрироваться".
     *
     * Этот слот вызывается, когда пользователь нажимает на ссылку для регистрации.
     */
    void onRegisterClicked();

    /*!
     * \brief Попытка входа в аккаунт.
     *
     * Этот слот вызывается при нажатии кнопки "Войти".
     */
    void attemptLogin();

    /*!
     * \brief Обработка ответа от сервера.
     *
     * Этот слот вызывается при получении ответа от сервера после попытки входа.
     */
    void handleServerResponse();

public:
    /*!
     * \brief Конструктор формы входа.
     *
     * \param socket Указатель на сокет для сетевого взаимодействия.
     * \param parent Указатель на родительский виджет (по умолчанию nullptr).
     */
    explicit LoginForm(QTcpSocket *socket, QWidget *parent = nullptr);

    /*!
     * \brief Получение логина пользователя.
     *
     * \return Строка с логином пользователя.
     */
    QString getLogin();

    /*!
     * \brief Подключение сокета.
     *
     * Устанавливает соединение с сервером через сокет.
     */
    void connectSocket();

signals:
    /*!
     * \brief Запрос на переход к форме регистрации.
     *
     * Этот сигнал отправляется, когда пользователь хочет зарегистрироваться.
     */
    void registerRequested();

    /*!
     * \brief Успешный вход в систему.
     *
     * Этот сигнал отправляется, когда вход выполнен успешно.
     */
    void loginSuccess();
};

#endif // LOGINFORM_H
