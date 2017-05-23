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
#include <stdio.h>
#include <QString>
#include <bitset>

//#include <elfio/elfio.hpp>
#include <udis86.h>
#include "myqgraphicsview.h"
#include "elf.h"
#include "machine_os.h"

using namespace std;

typedef struct elfArchitecture {
    int failure;
    bool show;
    Elf64_Xword flags;
    QString * name;
    int nameIndx;
    int type;
    Elf64_Off offset;
    Elf64_Off offsetHeader;
    Elf64_Half size;
    struct elfArchitecture * next;

} *TElfArchitecture;

typedef struct {
    TElfArchitecture First;
    TElfArchitecture strTabPtr;
    bool arch32;
    bool lsb;
    int filesize;
    int count;
    int strTab;
    int actual;
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
    int openFile(QString filename);
    void on_actionOpen_File_triggered();
    int saveFile(QString filename);
    void on_actionSave_File_triggered();
    void on_actionExit_triggered();
    void attChanged();
    void assChanged();

    int readHeader(fstream* file, Elf64_Ehdr* headInf, int controll);
    int readSegment(fstream* file, Elf32_Phdr* segment32, Elf64_Phdr* segment64, int offset, int controll);
    int readSection(fstream* file, Elf32_Shdr* section32, Elf64_Shdr* section64, int offset, int nmb, int controll, QString name);

    int setNames();
    void drawChart();
    void resizeEvent(QResizeEvent* event);

    int addRecord(TElfArchitecture* newItem);
    TElfArchitecture seekRecord(int orderNumber);

    void on_actionSave_As_triggered();

    void on_actionHelp_triggered();

private:
    Ui::MainWindow *ui;
    QTextEdit * assembleTE;
    QTextEdit * attributeTE;
    MyQGraphicsView * gv;
    TList elfArch;
    QString filename;
    bool attCh;
    bool assCh;
protected:

};

#endif // MAINWINDOW_H
