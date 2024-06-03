#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    unsigned int window_width = 500;
    unsigned int window_height = 500;

public:
    MainWindow(QWidget *parent = nullptr);

};
#endif // MAINWINDOW_H
