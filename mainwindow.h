#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "./src_libmodbus/modbus.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int ReadTimeFromICPDAS(QString IPaddr, QDateTime *icpDT);
    int SetTimeToICPDAS(QString IPaddr, QDateTime icpNewDT);
    void AddListToHistory();
    QTimer timer200ms;


public slots:
    void PushButtonReadOne();
    void PushButtonReadAll();
    void PushButtonSetOne();
    void PushButtonSetAll();
    void TimerEvent200ms();

   // void TextChanged(int iUzel,QString newText);



private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
