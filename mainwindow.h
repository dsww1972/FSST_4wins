#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QUdpSocket>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void nachricht();
    void empfangen();

private slots:
    void fct_restart();
    void on_ip_set_button_clicked();
    void on_turn_button_clicked();
    //void on_restart_button_clicked();

private:
    QUdpSocket *socket;
};
#endif // MAINWINDOW_H
