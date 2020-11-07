#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qlib/qlib.h"
#include "xtablewidget.h"
#include "xtreewidget.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void sigUpdateTable(plist<pmap<pstring,pstring>> lmdata, int keyid=0);
private slots:
    void slotThreadSendData();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
