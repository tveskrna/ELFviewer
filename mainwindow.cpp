#include "mainwindow.h"
#include "ui_mainwindow.h"

#define NO_TYPE 0
#define RELOCATABLE 1
#define EXECUTABLE 2
#define SHARED 3
#define CORE 4
#define SPECIFIC 5


#define HEADER 0
#define SEGMENT 1
#define SECTION 2

#define CHECK 0
#define READ 1

//using namespace ELFIO;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setBaseSize(1000, 600);

    this->assembleTE = new QTextEdit();
    this->assembleTE->setReadOnly(true);
    ui->horizontalLayout->layout()->addWidget(this->assembleTE);

    this->attributeTE = new QTextEdit();
    this->attributeTE->setReadOnly(true);
    ui->horizontalLayout->layout()->addWidget(this->attributeTE);

    this->gv = new MyQGraphicsView();
    this->gv->setMaximumWidth(329);
    ui->horizontalLayout->layout()->addWidget(this->gv);

    this->elfArch.First = NULL;

    connect(gv, SIGNAL(clickedOnMe(QPointF)), this, SLOT(clickedOnGraph(QPointF)));
    connect(this, SIGNAL(resizeWindow(int, int)), gv, SLOT(changeSize(int, int)));
    emit resizeWindow(1, 1);
}

void MainWindow::clickedOnGraph(QPointF pt)
{
    int up;
    int down;
    int pos = 0;
    while (true)
    {
        up   = 10 + pos * 35;
        down = 10 + pos * 35 + 25;
        if (pt.y() >= up && pt.y() <= down)
        {
            break;
        }
        pos++;
    }
    if (pos == HEADER)
    {
        Elf64_Ehdr header;
        fstream file ((this->filename).toStdString().c_str(), ios::in|ios::binary|ios::ate);
        if (file.is_open())
        {
            int result = readHeader(&file, &header, READ);
            if (result == 0)
            {
                this->gv->addRectangle("ELF Header", HEADER, HEADER);
            }
            else
            {
                if (result == 1) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.");
                if (result == 2) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.\n Incorrect file class.");
                if (result == 3) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.\n Incorrect file encoding.");
                if (result == 4) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.\n Incorrect fie version.");
                file.close();
                return;
            }
            file.close();
        }
        else
        {
            if (filename != "") QMessageBox::critical(this, "Wrong ELF file", "Can't open file: " + filename + " is not right");
        }
    }
    //rect->setRect(50 + type * 20, sectionNumb * 35 + 10, 200, 25);
    //this->assembleTE->append(QString("x = %1, y = %2").arg(pt.x()).arg(pt.y()));
}

void MainWindow::on_actionOpen_File_triggered()
{
    Elf64_Ehdr header;

    QString filename;

    this->assembleTE->clear();
    this->attributeTE->clear();
    this->gv->clearScene();

    filename = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*)"));
    this->filename = filename;

    fstream file (filename.toStdString().c_str(), ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        int result = readHeader(&file, &header, CHECK);
        if (result == 0)
        {

            this->gv->addRectangle("ELF Header", HEADER, HEADER);
            /*
            int i = 0;
            int j = 0;
            for(i = 1; i < 9; i++)
            {
                this->gv->addRectangle(QString("Segment %1").arg(i), SEGMENT, i + j);
                if (i == 3)
                {
                    for(j = 1; j < 3; j++)
                    this->gv->addRectangle(QString("Section %1").arg(j), SECTION, i + j);
                    j = j-1;
                }
            }*/

        }
        else
        {
            if (result == 1) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.");
            if (result == 2) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.\n Incorrect file class.");
            if (result == 3) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.\n Incorrect file encoding.");
            if (result == 4) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.\n Incorrect fie version.");
            file.close();
            return;
        }

        file.close();
    }
    else
    {
        if (filename != "") QMessageBox::critical(this, "Wrong ELF file", "Can't open file: " + filename + " is not right");
    }
    return;
}

int MainWindow::readHeader(fstream* file, Elf64_Ehdr* header, int controll)
{
    //check block
    (*file).seekg (0, ios::beg);   //start of file
    (*file).read ((char*) &(*header).e_ident, sizeof((*header).e_ident));

    //first 4 bytes are magical numbers
    if ((*header).e_ident[0] != 127) return 1;  //it's magical constant
    if ((*header).e_ident[1] != 'E') return 1;  //it's magical constant
    if ((*header).e_ident[2] != 'L') return 1;  //it's magical constant
    if ((*header).e_ident[3] != 'F') return 1;  //it's magical constant

    //5'th byte is class (invalid, 32-bit, 64-bit)
    if ((*header).e_ident[4] == 0) return 2;                        //invalid
    /*else if (header.e_ident[4] == 1) ((*headInf).classELF = 32); //32-bit
    else ((*headInf).classELF = 64);                                //64-bit
    */
    //6'th byte is data encoding (invalid, LSB, MSB)
    if ((*header).e_ident[5] == 0) return 3;                            //invalid
    /*else if (elfHeader.e_ident[5] == 0) ((*headInf).encodingELF = 1);   //LSB
    else ((*headInf).encodingELF = 2);                                  //MSB
    */
    //7'th byte is version of file (invalid, current)
    if ((*header).e_ident[6] == 0) return 4;   //invalid

    //other bytes are at this version of ELF format unused

    if (controll == READ)
    {
        this->attributeTE->clear();

        (*file).read ((char*) &(*header).e_type, sizeof((*header).e_type));
        (*file).read ((char*) &(*header).e_machine, sizeof((*header).e_machine));
        (*file).read ((char*) &(*header).e_version, sizeof((*header).e_version));
        (*file).read ((char*) &(*header).e_entry, sizeof((*header).e_entry));
        (*file).read ((char*) &(*header).e_phoff, sizeof((*header).e_phoff));
        (*file).read ((char*) &(*header).e_shoff, sizeof((*header).e_shoff));
        (*file).read ((char*) &(*header).e_flags, sizeof((*header).e_flags));
        (*file).read ((char*) &(*header).e_ehsize, sizeof((*header).e_ehsize));
        (*file).read ((char*) &(*header).e_phentsize, sizeof((*header).e_phentsize));
        (*file).read ((char*) &(*header).e_phnum, sizeof((*header).e_phnum));
        (*file).read ((char*) &(*header).e_shentsize, sizeof((*header).e_shentsize));
        (*file).read ((char*) &(*header).e_shnum, sizeof((*header).e_shnum));
        (*file).read ((char*) &(*header).e_shstrndx, sizeof((*header).e_shstrndx));

        switch (((*header).e_type)) {
        case NO_TYPE:
            this->attributeTE->append(QString("File type: no type"));
            break;
        case RELOCATABLE:
            this->attributeTE->append(QString("File type: relocatable"));
            break;
        case EXECUTABLE:
            this->attributeTE->append(QString("File type: executable"));
            break;
        case SHARED:
            this->attributeTE->append(QString("File type: shared"));
            break;
        case CORE:
            this->attributeTE->append(QString("File type: core"));
            break;
        default:
            this->attributeTE->append(QString("File type: procesor specific %1").arg((*header).e_type));
            break;
        }


        if ((*header).e_ident[4] == 1) this->attributeTE->append(QString("File class: 32-bit"));
        else this->attributeTE->append(QString("File class: 64-bit"));

        if ((*header).e_ident[5] == 1) this->attributeTE->append(QString("File encoding: LSB"));   //LSB
        else this->attributeTE->append(QString("File encoding: MSB"));   //MSB

        //(*file).read ((char*) &(*header).e_ehsize, sizeof((*header).e_ehsize));
        this->attributeTE->append(QString("Header size: %1\n").arg((*header).e_ehsize));

        //(*file).read ((char*) &(*header).e_phoff, sizeof((*header).e_phoff));
        //(*file).read ((char*) &(*header).e_phnum, sizeof((*header).e_phnum));
        //(*file).read ((char*) &(*header).e_phentsize, sizeof((*header).e_phentsize));
        if ((*header).e_phoff != 0)
        {
            this->attributeTE->append(QString("Programm header offset: %1").arg((*header).e_phoff));
            this->attributeTE->append(QString("Number of program header entries: %1").arg((*header).e_phnum));
            this->attributeTE->append(QString("Programm header entry size: %1\n").arg((*header).e_phentsize));
        }
        else this->attributeTE->append(QString("File has no programm header\n"));

        //(*file).read ((char*) &(*header).e_shoff, sizeof((*header).e_shoff));
        //(*file).read ((char*) &(*header).e_shnum, sizeof((*header).e_shnum));
        //(*file).read ((char*) &(*header).e_shentsize, sizeof((*header).e_shentsize));
        if ((*header).e_shoff != 0)
        {
            this->attributeTE->append(QString("Section header table offset: %1").arg((*header).e_shoff));
            this->attributeTE->append(QString("Number of section header table entries: %1").arg((*header).e_shnum));
            this->attributeTE->append(QString("Section header table entry size: %1\n").arg((*header).e_shentsize));
        }
        else this->attributeTE->append(QString("File has no section header table\n"));

        //(*file).read ((char*) &(*header).e_shstrndx, sizeof((*header).e_shstrndx));
        if ((*header).e_shstrndx != 0)
        {
            this->attributeTE->append(QString("Section name string table index: %1").arg((*header).e_shstrndx));
        }

    }
    else //controll == CHECK
    {
        TElfArchitecture item = (elfArchitecture*) malloc(sizeof(struct elfArchitecture));
        item->type = HEADER;
        item->offset = 0;
        item->next = NULL;

        this->elfArch.First = item;
    }

    return 0;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    /*qDebug() << this->gv->size().width();
    qDebug() << this->baseSize().width();
    qDebug() << this->gv->width();*/
    emit resizeWindow(this->gv->size().width(), this->gv->size().height());
}

MainWindow::~MainWindow()
{
    delete ui;
}
