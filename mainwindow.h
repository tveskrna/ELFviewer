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
#include "machine_os.h"

using namespace std;

typedef struct elfArchitecture {

  int type;
  Elf64_Off offset;
  Elf64_Off offsetHeader;
  Elf64_Half size;
  struct elfArchitecture * next;
  struct elfArchitecture * nextSeg;

} *TElfArchitecture;

typedef struct {
    TElfArchitecture First;
    bool arch32;
    bool lsb;
    int count;
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
    int readSegment(fstream* file, Elf32_Phdr* segment32, Elf64_Phdr* segment64, int offset, int controll);
    int readSection(fstream* file, Elf32_Shdr* section32, Elf64_Shdr* section64, int offset, int controll);

    void drawChart();
    void resizeEvent(QResizeEvent* event);

    int addRecord(TElfArchitecture* newItem);
    TElfArchitecture seekRecord(int orderNumber);

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
