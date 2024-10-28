#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>

/*!
 * \brief Класс формы настроек пользователя.
 *
 * Этот класс реализует интерфейс для изменения настроек пользователя,
 * таких как логин, никнейм и пароль.
 */
class SettingsForm : public QWidget
{
    Q_OBJECT

private:
    QTcpSocket *socket;              /*!< Указатель на сокет для сетевого взаимодействия */
    QString login;                   /*!< Логин пользователя */
    QString nickname;                /*!< Никнейм пользователя */
    QPushButton *backButton;         /*!< Кнопка для возврата на предыдущий экран */
    QPushButton *changeLoginButton;  /*!< Кнопка для изменения логина */
    QPushButton *saveLoginButton;    /*!< Кнопка для сохранения нового логина */
    QPushButton *changeNameButton;   /*!< Кнопка для изменения никнейма */
    QPushButton *saveNameButton;     /*!< Кнопка для сохранения нового никнейма */
    QPushButton *changePasswordButton;/*!< Кнопка для изменения пароля */
    QLabel *loginLabel;               /*!< Метка для отображения текущего логина */
    QLabel *nameLabel;                /*!< Метка для отображения текущего никнейма */
    QLineEdit *loginEdit;            /*!< Поле ввода для нового логина */
    QLineEdit *nameEdit;             /*!< Поле ввода для нового никнейма */

    /*!
     * \brief Проверяет, содержит ли логин только допустимые символы.
     *
     * \param login Логин пользователя.
     * \return true, если логин соответствует требованиям, иначе false.
     */
    bool loginContainsOnlyAllowedCharacters(const QString &login);

    /*!
     * \brief Проверяет, содержит ли пароль необходимые символы.
     *
     * \param password Пароль пользователя.
     * \return true, если пароль соответствует требованиям, иначе false.
     */
    bool passwordContainsRequiredCharacters(const QString &password);

private slots:
    /*!
     * \brief Сохраняет новый никнейм пользователя.
     *
     * Этот слот вызывается при нажатии на кнопку "Сохранить никнейм".
     */
    void saveName();

    /*!
     * \brief Сохраняет новый логин пользователя.
     *
     * Этот слот вызывается при нажатии на кнопку "Сохранить логин".
     */
    void saveLogin();

    /*!
     * \brief Включает редактирование поля никнейма.
     *
     * Этот слот вызывается при нажатии на кнопку "Изменить никнейм".
     */
    void enableNameEdit();

    /*!
     * \brief Включает редактирование поля логина.
     *
     * Этот слот вызывается при нажатии на кнопку "Изменить логин".
     */
    void enableLoginEdit();

    /*!
     * \brief Обрабатывает ответ от сервера.
     *
     * Этот слот вызывается при получении ответа от сервера после попытки изменить настройки.
     */
    void onServerResponse();

    /*!
     * \brief Обрабатывает клик по кнопке "Назад".
     *
     * Этот слот возвращает пользователя к предыдущему экрану (например, к мессенджеру).
     */
    void handleBackClick();

    /*!
     * \brief Включает возможность изменения пароля.
     *
     * Этот слот вызывается при нажатии на кнопку "Изменить пароль".
     */
    void enablePasswordChange();

public:
    /*!
      * \brief Конструктор формы настроек пользователя.
      *
      * Создает экземпляр формы и инициализирует его компоненты.
      *
      * \param socket Указатель на сокет для сетевого взаимодействия.
      * \param login Логин пользователя.
      * \param parent Указатель на родительский виджет (по умолчанию nullptr).
      */
    explicit SettingsForm(QTcpSocket *socket, QString login, QWidget *parent = nullptr);

    /*!
      * \brief Подключает сокет к серверу.
      *
      * Устанавливает соединение с сервером через сокет для обмена данными.
      */
    void connectSocket();

    /*!
      * \brief Запрашивает никнейм пользователя у сервера.
      *
      * Отправляет запрос на получение текущего никнейма пользователя с сервера.
      */
    void requestNickname();

signals:
    /*!
      * \brief Запрос на возврат к форме мессенджера.
      *
      * Этот сигнал отправляется, когда пользователь хочет вернуться к основному экрану мессенджера.
      */
    void backToMessengerFormRequested();

};

#endif // SETTINGSFORM_H
