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
#include <QTcpSocket>

/*!
 * \brief Класс формы регистрации.
 *
 * Этот класс реализует интерфейс для регистрации нового пользователя,
 * включая поля для ввода логина и пароля, а также кнопки для регистрации и возврата.
 */
class RegistrationForm : public QWidget
{
    Q_OBJECT

private:
    QLabel *registerLabel;                /*!< Метка для регистрации */
    QLabel *titleLabel;                   /*!< Заголовок формы */
    QLabel *imageLabel;                   /*!< Картинка, открывающая информацию */
    QLabel *opendEyeLabelPass;            /*!< Иконка открытого глаза для пароля */
    QLabel *closedEyeLabelPass;           /*!< Иконка закрытого глаза для пароля */
    QLabel *opendEyeLabelPassAgain;       /*!< Иконка открытого глаза для повторного ввода пароля */
    QLabel *closedEyeLabelPassAgain;      /*!< Иконка закрытого глаза для повторного ввода пароля */
    QLineEdit *loginEdit;                 /*!< Поле ввода логина */
    QLineEdit *passwordEdit;              /*!< Поле ввода пароля */
    QLineEdit *passwordEditAgain;         /*!< Поле для повторного ввода пароля */
    QPushButton *registerButton;          /*!< Кнопка для регистрации */
    QPushButton *backButton;              /*!< Кнопка для возврата на предыдущий экран */
    QTcpSocket *socket;                   /*!< Сокет для сетевого взаимодействия */

    /*!
     * \brief Фильтр событий.
     *
     * Этот метод обрабатывает события, происходящие в виджетах.
     *
     * \param target Указатель на объект, который отправил событие.
     * \param event Указатель на объект события.
     * \return true, если событие было обработано, иначе false.
     */
    bool eventFilter(QObject *target, QEvent *event);

    /*!
     * \brief Регистрирует нового пользователя.
     *
     * Этот метод отправляет данные о пользователе на сервер для регистрации.
     */
    void registerUser();

    /*!
     * \brief Проверяет, содержит ли пароль необходимые символы.
     *
     * \param password Пароль пользователя.
     * \return true, если пароль соответствует требованиям, иначе false.
     */
    bool passwordContainsRequiredCharacters(const QString &password);

    /*!
     * \brief Проверяет, содержит ли логин только допустимые символы.
     *
     * \param login Логин пользователя.
     * \return true, если логин соответствует требованиям, иначе false.
     */
    bool loginContainsOnlyAllowedCharacters(const QString &login);

private slots:
    /*!
     * \brief Обрабатывает клик по изображению.
     *
     * Этот слот вызывается при нажатии на изображение и может открывать дополнительную информацию.
     */
    void onImageLabelClicked();

    /*!
     * \brief Обрабатывает клик по кнопке "Назад".
     *
     * Этот слот вызывается при нажатии на кнопку "Назад" и возвращает пользователя к предыдущему экрану.
     */
    void backButtonClicked();

    /*!
     * \brief Обрабатывает ответ от сервера.
     *
     * Этот слот вызывается при получении ответа от сервера после попытки регистрации пользователя.
     */
    void handleServerResponse();

public:
    /*!
     * \brief Конструктор формы регистрации.
     *
     * Создает экземпляр формы и инициализирует его компоненты.
     *
     * \param socket Указатель на сокет для сетевого взаимодействия.
     * \param parent Указатель на родительский виджет (по умолчанию nullptr).
     */
    explicit RegistrationForm(QTcpSocket *socket, QWidget *parent = nullptr);

    /*!
     * \brief Подключает сокет к серверу.
     *
     * Устанавливает соединение с сервером через сокет для обмена данными.
     */
    void connectSocket();

signals:
    /*!
     * \brief Запрос на возврат к предыдущему экрану.
     *
     * Этот сигнал отправляется, когда пользователь хочет вернуться назад к форме входа или другой форме.
     */
    void backRequested();

};

#endif // REGISTRATIONFORM_H
