#ifndef CHATFORM_H
#define CHATFORM_H

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
 * \brief Класс формы чата.
 *
 * Этот класс реализует интерфейс для чата, позволяя пользователю отправлять и получать сообщения.
 */
class ChatForm : public QWidget
{
    Q_OBJECT

private:
    QString login;                /*!< Логин пользователя */
    QString chatId;              /*!< Идентификатор чата */
    QTcpSocket *socket;          /*!< Сокет для сетевого взаимодействия */
    QPushButton *backButton;     /*!< Кнопка "Назад" */
    QPushButton *blacklistButton; /*!< Кнопка для добавления пользователя в черный список */
    QListWidget *messageList;     /*!< Список сообщений чата */
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
     * \brief Обработчик добавления пользователя в черный список.
     *
     * Этот слот вызывается при нажатии на кнопку "Добавить в черный список".
     */
    void addToBlacklist();

    /*!
     * \brief Обработчик отправки сообщения.
     *
     * Этот слот вызывается при нажатии на кнопку "Отправить" и отправляет сообщение в чат.
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
     * \brief Конструктор формы чата.
     *
     * \param socket Указатель на сокет для сетевого взаимодействия.
     * \param login Логин пользователя.
     * \param chatId Идентификатор чата.
     * \param parent Указатель на родительский виджет (по умолчанию nullptr).
     */
    explicit ChatForm(QTcpSocket *socket, QString login, QString chatId, QWidget *parent = nullptr);

    /*!
     * \brief Подключение сокета.
     *
     * Устанавливает соединение с сервером через сокет.
     */
    void connectSocket();

    /*!
     * \brief Загрузка истории чата.
     *
     * Загружает предыдущие сообщения из чата для отображения пользователю.
     */
    void loadChatHistory();

signals:
    /*!
     * \brief Запрос на возврат к предыдущему экрану.
     *
     * Этот сигнал отправляется, когда пользователь хочет вернуться назад.
     */
    void backRequested();
};

#endif // CHATFORM_H
