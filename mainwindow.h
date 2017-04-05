#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QEvent>
#include <QtCore>
#include <QtGui>
#include <QDebug>
#include <QResizeEvent>

#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>

#include <iostream>
#include <sstream>
#include <string>
#include <QString>

#include "myqgraphicsview.h"
#include <elfio/elfio.hpp>

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
    void resizeWindow(int width, int height);

public slots:
    void clickedOnGraph(QPointF pt);

private slots:
    void on_actionOpen_File_triggered();
    void resizeEvent(QResizeEvent* event);

private:
    Ui::MainWindow *ui;
    QTextEdit * assembleTE;
    QTextEdit * attributeTE;
    MyQGraphicsView * gv;

protected:

};

#endif // MAINWINDOW_H
