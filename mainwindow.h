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
#include <fstream>
#include <sstream>
#include <string>
#include <QString>
#include <bitset>

//#include <elfio/elfio.hpp>
#include "myqgraphicsview.h"
#include "elf.h"

using namespace std;

typedef struct elfArchitecture {

  int type;
  Elf64_Off offset;
  struct elfArchitecture * next;

} *TElfArchitecture;

typedef struct {
    TElfArchitecture First;
} TList;

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
    int readHeader(fstream* file, Elf64_Ehdr* headInf, int controll);
    void resizeEvent(QResizeEvent* event);

private:
    Ui::MainWindow *ui;
    QTextEdit * assembleTE;
    QTextEdit * attributeTE;
    MyQGraphicsView * gv;
    TList elfArch;
    QString filename;
protected:

};

#endif // MAINWINDOW_H
