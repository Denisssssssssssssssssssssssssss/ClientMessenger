#ifndef GROUPCHATFORM_H
#define GROUPCHATFORM_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTcpSocket>
#include <QString>
#include <QLineEdit>

/*!
 * \brief Класс формы группового чата.
 *
 * Этот класс реализует интерфейс для группового чата, позволяя пользователям отправлять и получать сообщения.
 */
class GroupChatForm : public QWidget
{
    Q_OBJECT

private:
    QString login;                /*!< Логин пользователя */
    QString chatId;              /*!< Идентификатор группового чата */
    QTcpSocket *socket;          /*!< Сокет для сетевого взаимодействия */
    QPushButton *backButton;     /*!< Кнопка "Назад" */
    QPushButton *settingsButton;  /*!< Кнопка "Настройки" */
    QListWidget *messageList;     /*!< Список сообщений группового чата */
    QLineEdit *messageEdit;       /*!< Поле ввода сообщения */
    QPushButton *sendButton;      /*!< Кнопка отправки сообщения */

    /*!
     * \brief Добавляет сообщение в список сообщений.
     *
     * Этот метод добавляет новое сообщение в список сообщений с временной меткой.
     *
     * \param message Текст сообщения.
     * \param timestamp Временная метка сообщения.
     * \param isOwnMessage Указывает, является ли сообщение собственным (true) или полученным (false).
     */
    void appendMessageToList(const QString &message, const QString &timestamp, bool isOwnMessage);

private slots:
    /*!
     * \brief Обработчик нажатия кнопки "Назад".
     *
     * Этот слот вызывается при нажатии на кнопку "Назад" и возвращает пользователя к предыдущему экрану.
     */
    void goBack();

    /*!
     * \brief Обработчик открытия настроек.
     *
     * Этот слот вызывается при нажатии на кнопку "Настройки" и открывает меню настроек.
     */
    void openSettings();

    /*!
     * \brief Обработчик отправки сообщения.
     *
     * Этот слот вызывается при нажатии на кнопку "Отправить" и отправляет сообщение в групповой чат.
     */
    void sendMessage();

    /*!
     * \brief Обработчик готовности данных для чтения из сокета.
     *
     * Этот слот вызывается, когда данные доступны для чтения из сокета,
     * и обрабатывает входящие сообщения.
     */
    void onReadyRead();

public:
    /*!
     * \brief Конструктор формы группового чата.
     *
     * \param socket Указатель на сокет для сетевого взаимодействия.
     * \param login Логин пользователя.
     * \param chatId Идентификатор группового чата.
     * \param parent Указатель на родительский виджет (по умолчанию nullptr).
     */
    explicit GroupChatForm(QTcpSocket *socket, QString login, QString chatId, QWidget *parent = nullptr);

    /*!
     * \brief Подключение сокета.
     *
     * Устанавливает соединение с сервером через сокет.
     */
    void connectSocket();

    /*!
     * \brief Загрузка истории чата.
     *
     * Загружает предыдущие сообщения из группового чата для отображения пользователю.
     */
    void loadChatHistory();

signals:
    /*!
     * \brief Запрос на возврат к форме мессенджера.
     *
     * Этот сигнал отправляется, когда пользователь хочет вернуться к основному экрану мессенджера.
     */
    void backToMessengerFormRequested();

    /*!
     * \brief Запрос на открытие настроек.
     *
     * Этот сигнал отправляется, когда пользователь хочет открыть меню настроек.
     */
    void openSettingsRequested();

    /*!
     * \brief Сигнал для отправки идентификатора чата.
     *
     * Этот сигнал отправляется с идентификатором текущего группового чата.
     *
     * \param chatId Идентификатор группового чата.
     */
    void sendChatId(QString chatId);
};

#endif // GROUPCHATFORM_H
