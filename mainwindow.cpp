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

    this->gv = new MyQGraphicsView();
    this->gv->setMaximumWidth(329);
    ui->horizontalLayout->layout()->addWidget(this->gv);

    this->attributeTE = new QTextEdit();
    this->attributeTE->setReadOnly(true);
    ui->horizontalLayout->layout()->addWidget(this->attributeTE);

    this->assembleTE = new QTextEdit();
    this->assembleTE->setReadOnly(true);
    ui->horizontalLayout->layout()->addWidget(this->assembleTE);
    //this->assembleTE->setVisible(false);

    this->elfArch.First = NULL;

    connect(gv, SIGNAL(clickedOnMe(QPointF)), this, SLOT(clickedOnGraph(QPointF)));
    connect(this, SIGNAL(resizeWindow(int, int)), gv, SLOT(changeSize(int, int)));
    emit resizeWindow(1, 1);
}

//********************************operations-with-list**************************************
//******************************************************************************************

TElfArchitecture MainWindow::seekRecord(int orderNumber)
{
    TElfArchitecture result;

    if (this->elfArch.count > orderNumber)
    {
        result = elfArch.First;
        for (int i = 0; i < orderNumber; i++)
        {
            result = result->next;
        }
        return result;
    }
    else return NULL;
}

int MainWindow::addRecord(TElfArchitecture* newItem)
{
    TElfArchitecture * item;
    item = &elfArch.First;

    if ((*newItem)->type == SEGMENT)
    {
        while((*item)->nextSeg != NULL)
        {
            item = &(*item)->nextSeg;
        }
        (*item)->nextSeg = (*newItem);

        while((*item)->next != NULL)
        {
            item = &(*item)->next;
        }
        (*item)->next = (*newItem);
    }
    else        //SECTION
    {
        if ((*item)->nextSeg != NULL)
        {
            while(true)
            {
                if (((*newItem)->offset >= (*item)->offset) && ((*newItem)->offset < ((*item)->offset + (*item)->size)))
                {
                    while(true)
                    {
                        if ((*item)->next != NULL)
                        {
                            if ((*item)->next->type == SEGMENT)
                            {
                                (*newItem)->next = (*item)->next;
                                (*item)->next = (*newItem);
                                break;
                            }
                            else item = &(*item)->next;
                        }
                        else    //end of list
                        {
                            (*item)->next = (*newItem);
                            break;
                        }
                    }
                    break;
                }
                else
                {
                    if ((*item)->nextSeg != NULL)       //find segment which contains this section
                    {
                        item = &(*item)->nextSeg;
                    }
                    else (*item)->nextSeg = (*newItem); //section is not assigned to any segment
                }
            }

        }
        else
        {
            while(true)
            {
                if ((*item)->next != NULL)
                {
                    if ((*item)->next->type == SEGMENT)
                    {
                        (*newItem)->next = (*item)->next;
                        (*item)->next = (*newItem);
                        break;
                    }
                    else item = &(*item)->next;
                }
                else    //end of list
                {
                    (*item)->next = (*newItem);
                    break;
                }
            }
        }

    }

    this->elfArch.count++;
    return 0;
}


//********************************Read-ELF**************************************************
//******************************************************************************************


int MainWindow::readHeader(fstream* file, Elf64_Ehdr* header, int controll)
{
    (*file).read ((char*) &(*header).e_ident, sizeof((*header).e_ident));

    //first 4 bytes are magical numbers
    if ((*header).e_ident[0] != 127) return 1;  //it's magical constant
    if ((*header).e_ident[1] != 'E') return 1;  //it's magical constant
    if ((*header).e_ident[2] != 'L') return 1;  //it's magical constant
    if ((*header).e_ident[3] != 'F') return 1;  //it's magical constant

    //5'th byte is class (invalid, 32-bit, 64-bit)
    if ((*header).e_ident[4] == 0) return 2;            //invalid

    //6'th byte is data encoding (invalid, LSB, MSB)
    if ((*header).e_ident[5] == 0) return 3;            //invalid

    //7'th byte is version of file (invalid, current)
    if ((*header).e_ident[6] == 0) return 4;            //invalid

    //other bytes are at this version of ELF format unused

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

    if (controll == CHECK)
    {
        TElfArchitecture item = (elfArchitecture*) malloc(sizeof(struct elfArchitecture));
        item->type = HEADER;
        item->offset = 0;
        item->next = NULL;
        item->nextSeg = NULL;
        item->size = 64;

        this->elfArch.First = item;

        if ((*header).e_ident[4] == 1) this->elfArch.arch32 = true;
        else this->elfArch.arch32 = false;

        if ((*header).e_ident[5] == 1) this->elfArch.lsb = true;
        else this->elfArch.lsb = false;

        this->elfArch.count = 1;
    }

    return 0;
}

int MainWindow::readSegment(fstream* file, Elf32_Phdr* segment32, Elf64_Phdr* segment64, int offset, int controll)
{

    if (this->elfArch.arch32)
    {
        (*file).read ((char*) &(*segment32).p_type, sizeof((*segment32).p_type));
        (*file).read ((char*) &(*segment32).p_flags, sizeof((*segment32).p_flags));
        (*file).read ((char*) &(*segment32).p_offset, sizeof((*segment32).p_offset));
        (*file).read ((char*) &(*segment32).p_vaddr, sizeof((*segment32).p_vaddr));
        (*file).read ((char*) &(*segment32).p_paddr, sizeof((*segment32).p_paddr));
        (*file).read ((char*) &(*segment32).p_filesz, sizeof((*segment32).p_filesz));
        (*file).read ((char*) &(*segment32).p_memsz, sizeof((*segment32).p_memsz));
        (*file).read ((char*) &(*segment32).p_align, sizeof((*segment32).p_align));
    }
    else
    {
        (*file).read ((char*) &(*segment64).p_type, sizeof((*segment64).p_type));
        (*file).read ((char*) &(*segment64).p_flags, sizeof((*segment64).p_flags));
        (*file).read ((char*) &(*segment64).p_offset, sizeof((*segment64).p_offset));
        (*file).read ((char*) &(*segment64).p_vaddr, sizeof((*segment64).p_vaddr));
        (*file).read ((char*) &(*segment64).p_paddr, sizeof((*segment64).p_paddr));
        (*file).read ((char*) &(*segment64).p_filesz, sizeof((*segment64).p_filesz));
        (*file).read ((char*) &(*segment64).p_memsz, sizeof((*segment64).p_memsz));
        (*file).read ((char*) &(*segment64).p_align, sizeof((*segment64).p_align));
    }

    if (controll == CHECK)
    {
        TElfArchitecture item = (elfArchitecture*) malloc(sizeof(struct elfArchitecture));
        item->type = SEGMENT;

        if (this->elfArch.arch32) item->offset = segment32->p_offset;
        else item->offset = segment64->p_offset;

        item->offsetHeader = offset;

        item->next = NULL;
        item->nextSeg = NULL;

        if (this->elfArch.arch32) item->size = segment32->p_filesz;
        else item->size = segment64->p_filesz;

        addRecord(&item);
        //this->elfArch.count = this->elfArch.count + 1;
    }

    return 0;
}

QString readFlags(Elf32_Word value)
{
    QString result = "";

    if ((value & (1 << 0)) != 0) result = "Write";

    if ((value & (1 << 1)) != 0)
    {
        if (result == "") result = "Alloc";
        else result = result + ", Alloc";
    }

    if ((value & (1 << 2)) != 0)
    {
        if (result == "") result = "Executable";
        else result = result + ", Executable";
    }

    return result;
}

//***********************************Events*************************************************
//******************************************************************************************
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
        //HEADER
        file.seekg (0, ios::beg);   //start of file
        int result = readHeader(&file, &header, CHECK);

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

        //SEGMENT
        if (header.e_phoff != 0)    //there is program header
        {
            Elf32_Phdr segment32;
            Elf64_Phdr segment64;
            int result;

            int offset = header.e_phoff;
            file.seekg(offset, ios::beg);

            for (int i = 0; i < header.e_phnum; i++)
            {
                result = readSegment(&file, &segment32, &segment64, header.e_phoff + i * header.e_phentsize, CHECK);
                if (result == 0)
                {
                    this->gv->addRectangle("Segment", SEGMENT, i+1);
                }
            }
        }

        //SECTION

        file.close();
    }
    else
    {
        if (filename != "") QMessageBox::critical(this, "Wrong ELF file", "Can't open file: " + filename + " is not right");
    }
    return;
}

void MainWindow::clickedOnGraph(QPointF pt)
{
    TElfArchitecture record;
    int up, down, left, right;
    int pos = 0;

    while (pos < this->gv->height())
    {
        up   = 10 + pos * 35;
        down = 10 + pos * 35 + 25;
        if (pt.y() >= up && pt.y() <= down)
        {
            record = seekRecord(pos);
            if (record == NULL) break;

            left = 50 + record->type * 20;       //error
            right = 50 + record->type * 20 + 200;

            if ((pt.x() >= left) && (pt.x() <= right))
            {
                fstream file (filename.toStdString().c_str(), ios::in|ios::binary|ios::ate);
                if (file.is_open())
                {
                    int result;
                    file.seekg(record->offset, ios::beg);
                    this->attributeTE->clear();
                    this->assembleTE->clear();

                    switch (record->type) {
                    case HEADER:

                        Elf64_Ehdr header;
                        result = readHeader(&file, &header, READ);

                        switch (header.e_type) {
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
                                this->attributeTE->append(QString("File type: procesor specific %1").arg(header.e_type));
                                break;
                        }

                        if (header.e_ident[4] == 1) this->attributeTE->append(QString("File class: 32-bit"));
                        else this->attributeTE->append(QString("File class: 64-bit"));

                        if (header.e_ident[5] == 1) this->attributeTE->append(QString("File encoding: LSB"));   //LSB
                        else this->attributeTE->append(QString("File encoding: MSB"));   //MSB

                        this->attributeTE->append(QString("Header size: %1\n").arg(header.e_ehsize));

                        if (header.e_phoff != 0)
                        {
                            this->attributeTE->append(QString("Programm header offset: %1").arg(header.e_phoff));
                            this->attributeTE->append(QString("Number of program header entries: %1").arg(header.e_phnum));
                            this->attributeTE->append(QString("Programm header entry size: %1\n").arg(header.e_phentsize));
                        }
                        else this->attributeTE->append(QString("File has no programm header\n"));

                        if (header.e_shoff != 0)
                        {
                            this->attributeTE->append(QString("Section header table offset: %1").arg(header.e_shoff));
                            this->attributeTE->append(QString("Number of section header table entries: %1").arg(header.e_shnum));
                            this->attributeTE->append(QString("Section header table entry size: %1\n").arg(header.e_shentsize));
                        }
                        else this->attributeTE->append(QString("File has no section header table\n"));

                        if (header.e_shstrndx != 0)
                        {
                            this->attributeTE->append(QString("Section name string table index: %1").arg(header.e_shstrndx));
                        }

                        break;

                    case SEGMENT:

                        Elf32_Phdr segment32;
                        Elf64_Phdr segment64;
                        int result;

                        file.seekg(record->offsetHeader, ios::beg);

                        result = readSegment(&file, &segment32, &segment64, 0, READ);
                        if (result == 0)
                        {
                            if (this->elfArch.arch32)
                            {
                                switch (segment32.p_type)
                                {
                                    case PT_NULL:
                                        this->attributeTE->append(QString("Segment type: PT_NULL"));
                                    break;

                                    case PT_LOAD :
                                        this->attributeTE->append(QString("Segment type: PT_LOAD"));
                                    break;

                                    case PT_DYNAMIC :
                                        this->attributeTE->append(QString("Segment type: PT_DYNAMIC"));
                                    break;

                                    case PT_INTERP :
                                        this->attributeTE->append(QString("Segment type: PT_INTERP"));
                                    break;

                                    case PT_NOTE :
                                        this->attributeTE->append(QString("Segment type: PT_NOTE"));
                                    break;

                                    case PT_SHLIB :
                                        this->attributeTE->append(QString("Segment type: PT_SHLIB"));
                                    break;

                                    case PT_PHDR :
                                        this->attributeTE->append(QString("Segment type: PT_PHDR"));
                                    break;

                                    case PT_LOPROC :
                                        this->attributeTE->append(QString("Segment type: PT_LOPROC"));
                                    break;

                                    case PT_HIPROC :
                                        this->attributeTE->append(QString("Segment type: PT_HIPROC"));
                                    break;

                                    case PT_TLS:
                                        this->attributeTE->append(QString("Segment type: PT_TLS"));
                                    break;

                                    case PT_LOOS:
                                        this->attributeTE->append(QString("Segment type: PT_LOOS"));
                                    break;

                                    case PT_HIOS:
                                        this->attributeTE->append(QString("Segment type: PT_HIOS"));
                                    break;

                                    default:
                                        this->attributeTE->append(QString("Segment type: Procesor specific (%1)").arg(segment32.p_type));
                                    break;
                                }

                                this->attributeTE->append(QString("Segment offset: %1").arg(segment32.p_offset));
                                this->attributeTE->append(QString("Segment virtual address: %1").arg(segment32.p_vaddr));
                                this->attributeTE->append(QString("Segment physical address: %1").arg(segment32.p_paddr));
                                this->attributeTE->append(QString("Segment size in file: %1").arg(segment32.p_filesz));
                                this->attributeTE->append(QString("Segment size in memory: %1").arg(segment32.p_memsz));
                                QString flags = readFlags(segment32.p_flags);
                                this->attributeTE->append(QString("Segment flags: " + flags));
                                this->attributeTE->append(QString("Segment align: %1").arg(segment32.p_align));

                            }
                            else
                            {
                                switch (segment64.p_type)
                                {
                                    case PT_NULL:
                                        this->attributeTE->append(QString("Segment type: PT_NULL"));
                                    break;

                                    case PT_LOAD :
                                        this->attributeTE->append(QString("Segment type: PT_LOAD"));
                                    break;

                                    case PT_DYNAMIC :
                                        this->attributeTE->append(QString("Segment type: PT_DYNAMIC"));
                                    break;

                                    case PT_INTERP :
                                        this->attributeTE->append(QString("Segment type: PT_INTERP"));
                                    break;

                                    case PT_NOTE :
                                        this->attributeTE->append(QString("Segment type: PT_NOTE"));
                                    break;

                                    case PT_SHLIB :
                                        this->attributeTE->append(QString("Segment type: PT_SHLIB"));
                                    break;

                                    case PT_PHDR :
                                        this->attributeTE->append(QString("Segment type: PT_PHDR"));
                                    break;

                                    case PT_LOPROC :
                                        this->attributeTE->append(QString("Segment type: PT_LOPROC"));
                                    break;

                                    case PT_HIPROC :
                                        this->attributeTE->append(QString("Segment type: PT_HIPROC"));
                                    break;

                                    case PT_TLS:
                                        this->attributeTE->append(QString("Segment type: PT_TLS"));
                                    break;

                                    case PT_LOOS:
                                        this->attributeTE->append(QString("Segment type: PT_LOOS"));
                                    break;

                                    case PT_HIOS:
                                        this->attributeTE->append(QString("Segment type: PT_HIOS"));
                                    break;

                                    default:
                                        this->attributeTE->append(QString("Segment type: Procesor specific (%1)").arg(segment32.p_type));
                                    break;
                                }
                                this->attributeTE->append(QString("Segment offset: %1").arg(segment64.p_offset));
                                this->attributeTE->append(QString("Segment virtual address: %1").arg(segment64.p_vaddr));
                                this->attributeTE->append(QString("Segment physical address: %1").arg(segment64.p_paddr));
                                this->attributeTE->append(QString("Segment size in file: %1").arg(segment64.p_filesz));
                                this->attributeTE->append(QString("Segment size in memory: %1").arg(segment64.p_memsz));
                                QString flags = readFlags(segment64.p_flags);
                                this->attributeTE->append(QString("Segment flags: " + flags));
                                this->attributeTE->append(QString("Segment align: %1").arg(segment64.p_align));
                            }
                        }

                        break;

                    case SECTION:

                        break;

                    default:
                        break;
                    }
                    file.close();
                 }

                break;
            }
        }
        pos++;
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    emit resizeWindow(this->gv->size().width(), this->gv->size().height());
}

//******************************************************************************************
//******************************************************************************************

MainWindow::~MainWindow()
{
    delete ui;
}
