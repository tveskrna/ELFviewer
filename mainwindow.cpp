#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace ELFIO;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setBaseSize(1000, 600);

    this->assembleTE = new QTextEdit();
    ui->horizontalLayout->layout()->addWidget(this->assembleTE);

    this->attributeTE = new QTextEdit();
    ui->horizontalLayout->layout()->addWidget(this->attributeTE);

    this->gv = new MyQGraphicsView();
    this->gv->setMaximumWidth(329);
    ui->horizontalLayout->layout()->addWidget(this->gv);


    connect(gv, SIGNAL(clickedOnMe(QPointF)), this, SLOT(clickedOnGraph(QPointF)));
    connect(this, SIGNAL(resizeWindow(int, int)), gv, SLOT(changeSize(int, int)));
    emit resizeWindow(1, 1);
}

void MainWindow::clickedOnGraph(QPointF pt)
{
    this->assembleTE->append(QString("x = %1, y = %2").arg(pt.x()).arg(pt.y()));
    /*qDebug() << this->assembleTE->size().width();
    qDebug() << this->assembleTE->size().height();

    qDebug() << this->attributeTE->size().width();
    qDebug() << this->attributeTE->size().height();

    qDebug() << this->gv->
    qDebug() << this->gv->size().height();*/
    //qDebug() << this->gv->viewport()->size();

}

void MainWindow::on_actionOpen_File_triggered()
{
    elfio reader;
    QString filename;

    filename = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*)"));
    //QMessageBox::information(this, "Tittle", filename);

    if (!reader.load(filename.toStdString().c_str()))
    {
         QMessageBox::information(this, "Tittle", "Wrong ELF file");
    }

     //QMessageBox::information(this, "Tittle",reader.get_version());

    Elf_Half sec_num = reader.sections.size();
    emit resizeWindow(this->gv->size().width(), (sec_num + 1) * 35 + 25);
    for (int i = 0; i < sec_num; ++i)
    {
        string p = reader.sections[i]->get_name();
        this->gv->addRectangle("name", 0, i);
        /*ui->textEdit->append(p.c_str());
        const char * pole = reader.sections[i]->get_data();

        for(unsigned int i = 0; i < strlen(pole); i++)
        {
            ui->textEdit->append(((to_string((int) pole[i])) + " => " + (to_string((unsigned int) pole[i]))).c_str());
        }*/
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    qDebug() << this->gv->size().width();
    qDebug() << this->baseSize().width();
    qDebug() << this->gv->width();
    emit resizeWindow(this->gv->size().width(), this->gv->size().height());
}

MainWindow::~MainWindow()
{
    delete ui;
}
