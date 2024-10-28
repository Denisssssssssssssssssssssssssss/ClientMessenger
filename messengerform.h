#ifndef MESSENGERFORM_H
#define MESSENGERFORM_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTcpSocket>
#include <QString>
#include <QJsonArray>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QLabel>

/*!
 * \brief Класс формы мессенджера.
 *
 * Этот класс реализует интерфейс мессенджера, позволяя пользователю управлять чатами,
 * находить пользователей и настраивать параметры приложения.
 */
class MessengerForm : public QWidget
{
    Q_OBJECT

private:
    QPushButton *settingsButton;              /*!< Кнопка для открытия настроек */
    QPushButton *createGroupChatButton;       /*!< Кнопка для создания группового чата */
    QLineEdit *searchEdit;                     /*!< Поле для поиска пользователей */
    QListWidget *chatList;                     /*!< Список доступных чатов */
    QListWidget *userList;                     /*!< Список пользователей */
    QPushButton *logOutButton;                 /*!< Кнопка для выхода из системы */
    QTcpSocket *socket;                        /*!< Сокет для сетевого взаимодействия */
    QString login;                             /*!< Логин пользователя */
    QMenu *contextMenu;                        /*!< Контекстное меню для действий с чатами */
    QAction *deleteAction;                     /*!< Действие удаления чата */

private slots:
    /*!
     * \brief Открывает форму настроек.
     *
     * Этот слот вызывается при нажатии на кнопку "Настройки".
     */
    void openSettings();

    /*!
     * \brief Обрабатывает выход из системы.
     *
     * Этот слот вызывается при нажатии на кнопку "Выход".
     */
    void logOut();

    /*!
     * \brief Ищет пользователей по введенному тексту.
     *
     * Этот слот вызывается при изменении текста в поле поиска.
     */
    void findUsers();

    /*!
     * \brief Обновляет список пользователей.
     *
     * Этот слот обновляет список пользователей на основе полученного массива JSON.
     *
     * \param users Массив JSON с данными пользователей.
     */
    void updateUserList(QJsonArray users);

    /*!
     * \brief Обновляет список чатов.
     *
     * Этот слот обновляет список доступных чатов на основе полученного массива JSON.
     *
     * \param chats Массив JSON с данными чатов.
     */
    void updateChatList(QJsonArray chats);

    /*!
     * \brief Обрабатывает данные, готовые к чтению из сокета.
     *
     * Этот слот вызывается, когда данные доступны для чтения из сокета,
     * и обрабатывает входящие сообщения.
     */
    void onReadyRead();

    /*!
     * \brief Обрабатывает изменения текста в поле поиска.
     *
     * Этот слот вызывается при изменении текста в поле поиска пользователей.
     *
     * \param text Новый текст в поле поиска.
     */
    void onSearchTextChanged(const QString &text);

    /*!
     * \brief Открывает чат при выборе элемента из списка чатов.
     *
     * Этот слот вызывается при выборе элемента из списка чатов.
     *
     * \param item Указатель на выбранный элемент списка чатов.
     */
    void openChat(QListWidgetItem *item);

    /*!
     * \brief Обрабатывает клик по элементу списка чатов.
     *
     * Этот слот вызывается при клике на элемент списка чатов и открывает соответствующий чат.
     *
     * \param item Указатель на выбранный элемент списка чатов.
     */
    void onChatListItemClicked(QListWidgetItem *item);

    /*!
     * \brief Отображает контекстное меню по правому клику мыши.
     *
     * Этот слот вызывается при правом клике мыши и отображает контекстное меню с действиями над чатами.
     *
     * \param pos Позиция курсора мыши в момент клика.
     */
    void showContextMenu(const QPoint &pos);

    /*!
     * \brief Удаляет выбранный чат.
     *
     * Этот слот вызывается при выборе действия удаления из контекстного меню.
     */
    void deleteChat();

    /*!
     * \brief Создает новый групповой чат.
     *
     * Этот слот вызывается при нажатии на кнопку создания группового чата.
     */
    void createGroupChat();

    /*!
     * \brief Обрабатывает ответ о создании чата от сервера.
     *
     * Этот слот вызывается при получении ответа о создании нового чата от сервера.
     *
     * \param jsonObj Объект JSON с данными о созданном чате.
     */
    void handleChatCreationResponse(const QJsonObject &jsonObj);

public:
    /*!
      * \brief Конструктор формы мессенджера.
      *
      * Создает экземпляр формы мессенджера и инициализирует его компоненты.
      *
      * \param socket Указатель на сокет для сетевого взаимодействия.
      * \param login Логин пользователя.
      * \param parent Указатель на родительский виджет (по умолчанию nullptr).
      */
    explicit MessengerForm(QTcpSocket *socket, QString login, QWidget *parent = nullptr);

    /*!
      * \brief Подключает сокет к серверу.
      *
      * Устанавливает соединение с сервером через сокет для обмена данными.
      */
    void connectSocket();

    /*!
      * \brief Запрашивает список доступных чатов у сервера.
      *
      * Отправляет запрос на получение списка чатов для отображения пользователю.
      */
    void requestChatList();

signals:
    /*!
      * \brief Запрос на открытие формы настроек.
      *
      * Этот сигнал отправляется, когда требуется отобразить форму настроек.
      */
    void settingsRequested();

    /*!
      * \brief Запрос на выход из системы.
      *
      * Этот сигнал отправляется, когда пользователь хочет выйти из системы.
      */
    void logoutRequested();

    /*!
      * \brief Запрос на открытие чата.
      *
      * Этот сигнал отправляется с идентификатором и никнеймом пользователя для открытия соответствующего чата.
      *
      * \param chatId Идентификатор запрашиваемого чата.
      * \param userNickname Никнейм пользователя, участвующего в запрашиваемом чате.
      */
    void chatRequested(QString chatId, QString userNickname, QString chatType);

    /*!
      * \brief Получение идентификатора чата.
      *
      * Этот сигнал отправляется с идентификатором созданного или открытого чата.
      *
      * \param chatId Идентификатор полученного чата.
      */
    void chatIdReceived(QString chatId);

    /*!
      * \brief Уведомление о создании группового чата.
      *
      * Этот сигнал отправляется с идентификатором нового группового чата после его создания.
      *
      * \param chatId Идентификатор созданного группового чата.
      */
    void groupChatCreated(QString chatId);
};

#endif // MESSENGERFORM_H
