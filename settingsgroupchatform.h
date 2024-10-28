#ifndef SETTINGSGROUPCHATFORM_H
#define SETTINGSGROUPCHATFORM_H

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
 * \brief Класс формы настроек группового чата.
 *
 * Этот класс реализует интерфейс для управления настройками группового чата,
 * включая управление участниками, поиск пользователей и настройки чата.
 */
class SettingsGroupChatForm : public QWidget
{
    Q_OBJECT

private:
    QTcpSocket *socket;              /*!< Указатель на сокет для сетевого взаимодействия */
    QString nickname;                /*!< Никнейм пользователя */
    QPushButton *backButton;         /*!< Кнопка для возврата на предыдущий экран */
    QListWidget *participantsList;   /*!< Список участников группового чата */
    QListWidget *availableUsersList; /*!< Список доступных пользователей для добавления в чат */
    QLineEdit *searchEdit;           /*!< Поле для поиска пользователей */
    QString login;                   /*!< Логин пользователя */
    QString chatId;                  /*!< Идентификатор группового чата */
    QMenu *contextMenu;              /*!< Контекстное меню для действий с участниками чата */
    QAction *addAction;              /*!< Действие добавления пользователя в чат */

private slots:
    /*!
     * \brief Обрабатывает клик по кнопке "Назад".
     *
     * Этот слот возвращает пользователя к предыдущему экрану (например, к мессенджеру).
     */
    void handleBackClick();

    /*!
     * \brief Обрабатывает ответ от сервера.
     *
     * Этот слот вызывается при получении ответа от сервера после изменения настроек.
     */
    void onServerResponse();

    /*!
     * \brief Обрабатывает клик по кнопке удаления участника.
     *
     * Этот слот удаляет выбранного участника из группового чата.
     */
    void onRemoveButtonClick();

    /*!
     * \brief Ищет пользователей по введенному тексту.
     *
     * Этот слот вызывается при изменении текста в поле поиска.
     */
    void findUsers();

    /*!
     * \brief Обновляет список пользователей.
     *
     * Этот слот обновляет список доступных пользователей на основе полученного массива JSON.
     *
     * \param users Массив JSON с данными пользователей.
     */
    void updateUserList(QJsonArray users);


    /*!
     * \brief Обрабатывает изменения текста в поле поиска.
     *
     * Этот слот вызывается при изменении текста в поле поиска пользователей.
     *
     * \param text Новый текст в поле поиска.
     */
    void onSearchTextChanged(const QString &text);

    /*!
     * \brief Обрабатывает клик по кнопке добавления пользователя.
     *
     * Этот слот добавляет выбранного пользователя в групповой чат.
     */
    void onAddButtonClick();

    /*!
     * \brief Отображает контекстное меню по правому клику мыши.
     *
     * Этот слот вызывается при правом клике мыши и отображает контекстное меню с действиями над участниками чата.
     *
     * \param pos Позиция курсора мыши в момент клика.
     */
    void showContextMenu(const QPoint &pos);

public:
    /*!
      * \brief Конструктор формы настроек группового чата.
      *
      * Создает экземпляр формы и инициализирует его компоненты.
      *
      * \param socket Указатель на сокет для сетевого взаимодействия.
      * \param login Логин пользователя.
      * \param chatId Идентификатор группового чата.
      * \param parent Указатель на родительский виджет (по умолчанию nullptr).
      */
    explicit SettingsGroupChatForm(QTcpSocket *socket, QString login, QString chatId, QWidget *parent = nullptr);

    /*!
      * \brief Подключает сокет к серверу.
      *
      * Устанавливает соединение с сервером через сокет для обмена данными.
      */
    void connectSocket();

signals:
    /*!
      * \brief Запрос на возврат к форме мессенджера.
      *
      * Этот сигнал отправляется, когда пользователь хочет вернуться к основному экрану мессенджера.
      */
    void backToMessengerFormRequested();
};

#endif // SETTINGSGROUPCHATFORM_H
