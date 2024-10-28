#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "loginform.h"
#include "registrationform.h"
#include "nicknameform.h"
#include "messengerform.h"
#include "settingsform.h"
#include "chatform.h"
#include "groupchatform.h"
#include "settingsgroupchatform.h"
#include <QMainWindow>
#include <QJsonObject>
#include <QString>

/*!
 * \brief Главный класс окна приложения.
 *
 * Этот класс управляет основным окном приложения и переключением между различными формами,
 * такими как вход, регистрация, мессенджер и чаты.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    LoginForm *loginForm;                /*!< Указатель на форму входа */
    RegistrationForm *registrationForm;  /*!< Указатель на форму регистрации */
    NicknameForm *nicknameForm;          /*!< Указатель на форму выбора никнейма */
    MessengerForm *messengerForm;        /*!< Указатель на мессенджер */
    SettingsForm *settingsForm;          /*!< Указатель на форму настроек */
    ChatForm *chatForm;                  /*!< Указатель на форму чата */
    GroupChatForm *groupchatForm;        /*!< Указатель на форму группового чата */
    SettingsGroupChatForm *settingsgroupchatForm; /*!< Указатель на форму настроек группового чата */

    unsigned int window_width = 500;     /*!< Ширина окна */
    unsigned int window_height = 500;    /*!< Высота окна */

    QTcpSocket *socket;                   /*!< Сокет для сетевого взаимодействия */
    QString login;                        /*!< Логин пользователя */
    QString chatId;                      /*!< Идентификатор текущего чата */

    /*!
     * \brief Проверяет, является ли никнейм новым для пользователя.
     *
     * Этот метод используется для проверки статуса никнейма при входе или регистрации.
     */
    void isNicknameNewUser();

    /*!
     * \brief Устанавливает идентификатор чата.
     *
     * Этот метод устанавливает идентификатор текущего чата.
     *
     * \param chatId Идентификатор чата.
     */
    void setChatId(QString chatId);

private slots:
    /*!
     * \brief Отображает форму входа.
     *
     * Этот слот вызывается для отображения формы входа в систему.
     */
    void showLoginForm();

    /*!
     * \brief Отображает форму регистрации.
     *
     * Этот слот вызывается для отображения формы регистрации нового пользователя.
     */
    void showRegistrationForm();

    /*!
     * \brief Отображает мессенджер.
     *
     * Этот слот вызывается для отображения основной формы мессенджера.
     */
    void showMessengerForm();

    /*!
     * \brief Отображает форму выбора никнейма.
     *
     * Этот слот вызывается для отображения формы выбора никнейма пользователем.
     */
    void showNicknameForm();

    /*!
     * \brief Отображает форму настроек группового чата.
     *
     * Этот слот вызывается для отображения настроек группового чата.
     */
    void showSettingsGroupChatForm();

    /*!
     * \brief Обрабатывает успешный вход в систему.
     *
     * Этот слот вызывается при успешном входе пользователя в систему.
     */
    void onLoginSuccess();

    /*!
     * \brief Обрабатывает статус никнейма.
     *
     * Этот слот вызывается для получения статуса никнейма пользователя.
     */
    void receiveNicknameStatus();

    /*!
     * \brief Отображает форму настроек приложения.
     *
     * Этот слот вызывается для отображения формы настроек приложения.
     */
    void showSettingsForm();

    /*!
     * \brief Обрабатывает выход из системы.
     *
     * Этот слот вызывается при выходе пользователя из системы.
     */
    void handleLogout();

    /*!
     * \brief Отображает форму чата.
     *
     * Этот слот вызывается для отображения формы чата с указанным идентификатором и никнеймом пользователя.
     *
     * \param chatId Идентификатор чата, который нужно открыть.
     * \param userNickname Никнейм пользователя, участвующего в чате.
     * \param chatType Тип чата (индивидуальный или групповой).
     */
    void showChatForm(QString chatId, QString userNickname, QString chatType);

    /*!
     * \brief Отображает форму группового чата.
     *
     * Этот слот вызывается для отображения формы группового чата с указанным идентификатором и никнеймом пользователя.
     *
     * \param chatId Идентификатор группового чата, который нужно открыть.
     * \param userNickname Никнейм пользователя, участвующего в групповом чате.
     */
    void showGroupChatForm(QString chatId, QString userNickname);

    /*!
     * \brief Обрабатывает запрос на открытие чата.
     *
     * Этот слот вызывается при запросе открытия чата с указанным идентификатором и никнеймом пользователя.
     *
     * \param chatId Идентификатор запрашиваемого чата.
     * \param userNickname Никнейм пользователя, участвующего в запрашиваемом чате.
     * \param chatType Тип запрашиваемого чата (индивидуальный или групповой).
     */
    void onChatRequested(QString chatId, QString userNickname, QString chatType);

protected:
    /*!
     * \brief Обрабатывает событие закрытия окна.
     *
     * Этот метод переопределяет стандартное поведение закрытия окна,
     * позволяя выполнить дополнительные действия перед закрытием приложения.
     *
     * \param event Указатель на событие закрытия окна.
     */
    void closeEvent(QCloseEvent *event) override;

public:
    /*!
      * \brief Конструктор главного окна приложения.
      *
      * Создает экземпляр главного окна и инициализирует его компоненты.
      *
      * \param socket Указатель на сокет для сетевого взаимодействия с сервером.
      * \param parent Указатель на родительский виджет (по умолчанию nullptr).
      */
    MainWindow(QTcpSocket* socket, QWidget *parent = nullptr);

signals:
    /*!
      * \brief Запрос на переключение на форму входа.
      *
      * Этот сигнал отправляется, когда требуется отобразить форму входа.
      */
    void loginRequested();

    /*!
      * \brief Запрос на проверку статуса никнейма.
      *
      * Этот сигнал отправляется с никнеймом для проверки его статуса.
      *
      * \param login Никнейм пользователя для проверки статуса.
      */
    void checkNicknameStatus(const QString& login);

};

#endif // MAINWINDOW_H
