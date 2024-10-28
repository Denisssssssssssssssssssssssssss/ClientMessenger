#ifndef NICKNAMEFORM_H
#define NICKNAMEFORM_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>
#include <QTcpSocket>

/*!
 * \brief Класс формы выбора никнейма.
 *
 * Этот класс реализует интерфейс для ввода и сохранения никнейма пользователя.
 */
class NicknameForm : public QWidget
{
    Q_OBJECT

private:
    QLabel *instructionsLabel; /*!< Метка с инструкциями для пользователя */
    QLineEdit *nicknameEdit;   /*!< Поле ввода для никнейма */
    QPushButton *saveButton;   /*!< Кнопка для сохранения никнейма */
    QTcpSocket *socket;        /*!< Сокет для сетевого взаимодействия */
    QString login;             /*!< Логин пользователя */

private slots:
    /*!
     * \brief Сохраняет никнейм пользователя.
     *
     * Этот слот вызывается при нажатии на кнопку "Сохранить" и отправляет никнейм на сервер.
     */
    void saveNickname();

    /*!
     * \brief Обрабатывает ответ от сервера.
     *
     * Этот слот вызывается при получении ответа от сервера после попытки обновления никнейма.
     */
    void handleServerResponse();

public:
    /*!
     * \brief Конструктор формы выбора никнейма.
     *
     * Создает экземпляр формы и инициализирует его компоненты.
     *
     * \param socket Указатель на сокет для сетевого взаимодействия.
     * \param login Логин пользователя.
     * \param parent Указатель на родительский виджет (по умолчанию nullptr).
     */
    explicit NicknameForm(QTcpSocket *socket, QString login, QWidget *parent = nullptr);

    /*!
     * \brief Подключает сокет к серверу.
     *
     * Устанавливает соединение с сервером через сокет для обмена данными.
     */
    void connectSocket();

signals:
    /*!
     * \brief Сигнал об успешном обновлении никнейма.
     *
     * Этот сигнал отправляется, когда никнейм успешно обновлен на сервере.
     */
    void nicknameUpdated(); // Сигнал об успешном обновлении никнейма

};

#endif // NICKNAMEFORM_H
