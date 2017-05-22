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
#define READ_H 2

//using namespace ELFIO;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("ELF viewer");
    this->setBaseSize(1000, 600);

    this->gv = new MyQGraphicsView();
    this->gv->setMaximumWidth(329);
    this->gv->setMinimumWidth(329);
    this->gv->setMinimumHeight(300);
    ui->horizontalLayout->layout()->addWidget(this->gv);

    this->attributeTE = new QTextEdit();
    this->attributeTE->setMinimumWidth(320);
    this->attributeTE->setMaximumWidth(350);
    //this->attributeTE->setReadOnly(true);
    ui->horizontalLayout->layout()->addWidget(this->attributeTE);

    this->assembleTE = new QTextEdit();
    this->assembleTE->setMinimumWidth(320);
    //this->assembleTE->setReadOnly(true);
    ui->horizontalLayout->layout()->addWidget(this->assembleTE);
    //this->assembleTE->setVisible(false);

    this->elfArch.First = NULL;
    this->elfArch.count = 0;
    this->filename = "";

    connect(gv, SIGNAL(clickedOnMe(QPointF)), this, SLOT(clickedOnGraph(QPointF)));
    connect(this, SIGNAL(resizeWindow(int, int)), gv, SLOT(changeSize(int, int)));

    connect(this->attributeTE, SIGNAL(textChanged()),this, SLOT(attChanged()));
    connect(this->assembleTE, SIGNAL(textChanged()),this, SLOT(assChanged()));

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

    if ((*newItem)->type == SEGMENT)    //add segments
    {
        while((*item)->next != NULL)
        {
            item = &(*item)->next;
        }
        (*item)->next = (*newItem);
        this->elfArch.count++;
    }

    if ((*newItem)->type == SECTION)    //add section
    {
        bool added = false;
        while((*item)->next != NULL)
        {
            if ((*item)->type == SEGMENT)
            {
                if (((*newItem)->offset >= (*item)->offset) && ((*newItem)->offset < ((*item)->offset + (*item)->size)))
                {
                    //create new record
                    TElfArchitecture tmpItem = (elfArchitecture*) malloc(sizeof(struct elfArchitecture));
                    tmpItem->failure = (*newItem)->failure;
                    tmpItem->show = (*newItem)->show;
                    tmpItem->flags = (*newItem)->flags;
                    tmpItem->name = new QString("Section");
                    tmpItem->nameIndx = (*newItem)->nameIndx;
                    tmpItem->type = SECTION;

                    tmpItem->offset = (*newItem)->offset;
                    tmpItem->offsetHeader = (*newItem)->offsetHeader;

                    tmpItem->next = NULL;

                    tmpItem->size = (*newItem)->size;

                    //add new record
                    while((*item)->next != NULL && (*item)->next->type != SEGMENT)
                    {
                        item = &(*item)->next;
                    }
                    tmpItem->next = (*item)->next;
                    (*item)->next = tmpItem;
                    this->elfArch.count++;
                    added = true;
                }
            }
            item = &(*item)->next;
        }
        if (added == false)
        {
            (*item)->next = (*newItem);
            this->elfArch.count++;
        }
    }

    return 0;
}

//********************************Read-ELF**************************************************
//******************************************************************************************
QString readFlags(Elf64_Xword value)
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

    if (result == "") result = "None";
    return result;
}

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
        item->name = new QString("ELF Header");
        item->failure = 0;
        item->show = true;
        item->flags = (*header).e_flags;
        item->nameIndx = 0;
        item->type = HEADER;
        item->offset = 0;
        item->offsetHeader = 0;
        item->next = NULL;
        item->size = (*header).e_ehsize;

        this->elfArch.First = item;

        if ((*header).e_ident[4] == 1) this->elfArch.arch32 = true;
        else this->elfArch.arch32 = false;

        if ((*header).e_ident[5] == 1) this->elfArch.lsb = true;
        else this->elfArch.lsb = false;

        this->elfArch.strTab = (*header).e_shstrndx;

        this->elfArch.count = 1;
        this->elfArch.actual = 0;
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
        int mistake = 0;
        if (this->elfArch.arch32)
        {
            if (((*segment32).p_offset + (*segment32).p_filesz) > this->elfArch.filesize) mistake = 1;
        }
        else
        {
            if (((*segment64).p_offset + (*segment64).p_filesz) > this->elfArch.filesize) mistake = 1;
        }

        TElfArchitecture item = (elfArchitecture*) malloc(sizeof(struct elfArchitecture));
        item->name = new QString("Segment");
        item->show = true;
        item->failure = mistake;

        if (this->elfArch.arch32) item->flags = segment32->p_flags;
        else item->flags = segment64->p_flags;

        if (this->elfArch.arch32) item->nameIndx = (*segment32).p_type;
        else item->nameIndx = (*segment64).p_type;

        item->type = SEGMENT;

        if (this->elfArch.arch32) item->offset = segment32->p_offset;
        else item->offset = segment64->p_offset;

        item->offsetHeader = offset;

        item->next = NULL;

        if (this->elfArch.arch32) item->size = segment32->p_filesz;
        else item->size = segment64->p_filesz;

        addRecord(&item);
        return mistake;
        //this->elfArch.count = this->elfArch.count + 1;
    }

    return 0;
}

int MainWindow::readSection(fstream* file, Elf32_Shdr* section32, Elf64_Shdr* section64, int offset, int nmb, int controll, QString nameSection)
{

    if (this->elfArch.arch32)
    {
        (*file).read ((char*) &(*section32).sh_name, sizeof((*section32).sh_name));
        (*file).read ((char*) &(*section32).sh_type, sizeof((*section32).sh_type));
        (*file).read ((char*) &(*section32).sh_flags, sizeof((*section32).sh_flags));
        (*file).read ((char*) &(*section32).sh_addr, sizeof((*section32).sh_addr));
        (*file).read ((char*) &(*section32).sh_offset, sizeof((*section32).sh_offset));
        (*file).read ((char*) &(*section32).sh_size, sizeof((*section32).sh_size));
        (*file).read ((char*) &(*section32).sh_link, sizeof((*section32).sh_link));
        (*file).read ((char*) &(*section32).sh_info, sizeof((*section32).sh_info));
        (*file).read ((char*) &(*section32).sh_addralign, sizeof((*section32).sh_addralign));
        (*file).read ((char*) &(*section32).sh_entsize, sizeof((*section32).sh_entsize));
    }
    else
    {
        (*file).read ((char*) &(*section64).sh_name, sizeof((*section64).sh_name));
        (*file).read ((char*) &(*section64).sh_type, sizeof((*section64).sh_type));
        (*file).read ((char*) &(*section64).sh_flags, sizeof((*section64).sh_flags));
        (*file).read ((char*) &(*section64).sh_addr, sizeof((*section64).sh_addr));
        (*file).read ((char*) &(*section64).sh_offset, sizeof((*section64).sh_offset));
        (*file).read ((char*) &(*section64).sh_size, sizeof((*section64).sh_size));
        (*file).read ((char*) &(*section64).sh_link, sizeof((*section64).sh_link));
        (*file).read ((char*) &(*section64).sh_info, sizeof((*section64).sh_info));
        (*file).read ((char*) &(*section64).sh_addralign, sizeof((*section64).sh_addralign));
        (*file).read ((char*) &(*section64).sh_entsize, sizeof((*section64).sh_entsize));
    }

    if (controll == CHECK)
    {
        int mistake = 0;
        if (this->elfArch.arch32)
        {
            if (((*section32).sh_offset + (*section32).sh_size) > this->elfArch.filesize) mistake = 1;
        }
        else
        {
            if (((*section64).sh_offset + (*section64).sh_size) > this->elfArch.filesize) mistake = 1;
        }

        TElfArchitecture item = (elfArchitecture*) malloc(sizeof(struct elfArchitecture));
        item->name = new QString("Section");
        item->failure = mistake;
        item->show = true;
        if (this->elfArch.arch32) item->flags =(*section32).sh_flags;
        else item->flags = (*section64).sh_flags;

        if (this->elfArch.arch32) item->nameIndx = (*section32).sh_name;
        else item->nameIndx = (*section64).sh_name;

        item->type = SECTION;

        if (this->elfArch.arch32) item->offset = section32->sh_offset;
        else item->offset = section64->sh_offset;

        item->offsetHeader = offset;

        item->next = NULL;

        if (this->elfArch.arch32) item->size = section32->sh_size;
        else item->size = section64->sh_size;

        if (nmb == this->elfArch.strTab)
        {
            this->elfArch.strTabPtr = item;
        }

        addRecord(&item);
        return mistake;
        //this->elfArch.count = this->elfArch.count + 1;
    }

    if (controll == READ)
    {
        unsigned char * buffer;

        if (elfArch.arch32) buffer = (unsigned char*) malloc(sizeof(unsigned char) * section32->sh_size);        //alloc
        else buffer = (unsigned char*) malloc(sizeof(unsigned char) * section64->sh_size);

        if (elfArch.arch32) (*file).seekg(section32->sh_offset, ios::beg);      //seek
        else (*file).seekg(section64->sh_offset, ios::beg);

        if (elfArch.arch32) (*file).read ((char*) buffer, section32->sh_size);  //read
        else (*file).read ((char*) buffer, section64->sh_size);

        QString flags;
        if (elfArch.arch32) flags = readFlags(section32->sh_flags);
        else flags = readFlags(section64->sh_flags);

        if (flags.indexOf("Executable", 0) != -1)
        {
            ud_t ud_obj;

            ud_init(&ud_obj);
            ud_set_input_buffer(&ud_obj, buffer, section64->sh_size);

            if (elfArch.arch32) ud_set_mode(&ud_obj, 32);
            else ud_set_mode(&ud_obj, 64);

            ud_set_syntax(&ud_obj, UD_SYN_INTEL);

            QString str = "";

            while (ud_disassemble(&ud_obj)) {
                this->assembleTE->append(ud_insn_asm(&ud_obj));
            }
        }
        else
        {
            QString str = "";
            QString strH = "";
            int type;
            char c;
            if (elfArch.arch32)
            {
                (*file).seekg(section32->sh_offset, ios::beg);
                type = section32->sh_type;
                if (type > SHT_NUM)
                {
                    if (nameSection.indexOf(".hash", 0) != -1) type = SHT_HASH;
                    else if (nameSection.indexOf(".rela", 0) != -1) type = SHT_RELA;
                    else if (nameSection.indexOf(".rel", 0) != -1) type = SHT_REL;
                }

                switch (type)
                {
                    case SHT_SYMTAB:
                        Elf32_Sym symTab;

                        while ((*file).tellg() < section32->sh_offset + section32->sh_size)
                        {
                            (*file).read ((char*) &symTab.st_name, sizeof(symTab.st_name));
                            (*file).read ((char*) &symTab.st_value, sizeof(symTab.st_value));
                            (*file).read ((char*) &symTab.st_size, sizeof(symTab.st_size));
                            (*file).read ((char*) &symTab.st_info, sizeof(symTab.st_info));
                            (*file).read ((char*) &symTab.st_other, sizeof(symTab.st_other));
                            (*file).read ((char*) &symTab.st_shndx, sizeof(symTab.st_shndx));

                            this->assembleTE->append(QString("ST_name: %1").arg(symTab.st_name));
                            this->assembleTE->append(QString("ST_value: %1").arg(symTab.st_value));
                            this->assembleTE->append(QString("ST_size: %1").arg(symTab.st_size));
                            this->assembleTE->append(QString("ST_info: %1").arg(symTab.st_info));
                            this->assembleTE->append(QString("ST_other: %1").arg(symTab.st_other));
                            this->assembleTE->append(QString("ST_shndx: %1\n").arg(symTab.st_shndx));
                        }

                    break;
                    case SHT_RELA:
                        Elf32_Rela relaTable;

                        while ((*file).tellg() < section32->sh_offset + section32->sh_size)
                        {
                            (*file).read ((char*) &relaTable.r_offset, sizeof(relaTable.r_offset));
                            (*file).read ((char*) &relaTable.r_info, sizeof(relaTable.r_info));
                            (*file).read ((char*) &relaTable.r_addend, sizeof(relaTable.r_addend));

                            this->assembleTE->append(QString("R_offset: %1").arg(relaTable.r_offset));
                            this->assembleTE->append(QString("R_info: %1").arg(relaTable.r_info));
                            this->assembleTE->append(QString("R_addend: %1\n").arg(relaTable.r_addend));
                        }

                    break;
                    case SHT_HASH:
                        Elf32_Word nbucket;
                        Elf32_Word nchain;
                        Elf32_Word entry;

                        (*file).read ((char*) &nbucket, sizeof(nbucket));
                        (*file).read ((char*) &nchain, sizeof(nchain));

                        this->assembleTE->append(QString("%1").arg(nbucket));
                        this->assembleTE->append(QString("%1").arg(nchain));

                        for (int i = 0; i < nbucket; i++)
                        {
                            (*file).read ((char*) &entry, sizeof(entry));
                            strH = strH + QString("%1").arg(entry);
                        }

                        this->assembleTE->append(strH);
                        strH = "";

                        for (int i = 0; i < nchain; i++)
                        {
                            (*file).read ((char*) &entry, sizeof(entry));
                            strH = strH + QString("%1").arg(entry);
                        }

                        this->assembleTE->append(strH);

                    break;                        
                    case SHT_DYNAMIC:

                        Elf32_Sword tag;
                        Elf32_Sword val;
                        Elf32_Addr ptr;

                        while ((*file).tellg() < section32->sh_offset + section32->sh_size)
                        {
                            (*file).read ((char*) &tag, sizeof(tag));
                            this->assembleTE->append(QString("D_TAG: %1").arg(tag));

                            switch(tag)
                            {
                                case 1:
                                case 2:
                                case 8:
                                case 9:
                                case 10:
                                case 11:
                                case 14:
                                case 15:
                                case 18:
                                case 19:
                                case 20:
                                    //d_val
                                    (*file).read ((char*) &val, sizeof(val));
                                    this->assembleTE->append(QString("D_VAL: %1").arg(val));
                                break;

                                case 3:
                                case 4:
                                case 5:
                                case 6:
                                case 7:
                                case 12:
                                case 13:
                                case 17:
                                case 21:
                                case 23:
                                    //d_ptr
                                    (*file).read ((char*) &ptr, sizeof(ptr));
                                    this->assembleTE->append(QString("D_PTR: %1").arg(ptr));
                                break;

                                default:
                                    //do nothing
                                break;
                            }
                            this->assembleTE->append(QString(""));
                        }


                    break;
                    case SHT_NOTE:
                        Elf32_Word name;
                        Elf32_Word descsz;
                        Elf32_Word type;
                        Elf32_Word desc;


                        while ((*file).tellg() < section32->sh_offset + section32->sh_size)
                        {
                            (*file).read ((char*) &name, sizeof(name));
                            (*file).read ((char*) &descsz, sizeof(descsz));
                            (*file).read ((char*) &type, sizeof(type));

                            this->assembleTE->append(QString("Name: %1").arg(name));
                            this->assembleTE->append(QString("DescSZ: %1").arg(descsz));
                            this->assembleTE->append(QString("DescSZ: %1").arg(type));

                            char bufferName [name];
                            (*file).read ((char*) &bufferName, name);

                            str = "";
                            for (int j = 0; j < name; j++)
                            {
                                c = bufferName[j];
                                str = str + c;
                            }

                            this->assembleTE->append(QString(str));

                            if (descsz > 0)
                            {
                                for (int j = 0; j < descsz/4; j++)
                                {
                                    (*file).read ((char*) &desc, sizeof(desc));
                                    this->assembleTE->append(QString("Desc: %1").arg(desc));
                                }
                            }
                        }


                    break;
                    case SHT_REL:
                        Elf32_Rel relTable;

                        while ((*file).tellg() < section32->sh_offset + section32->sh_size)
                        {
                            (*file).read ((char*) &relTable.r_offset, sizeof(relTable.r_offset));
                            (*file).read ((char*) &relTable.r_info, sizeof(relTable.r_info));

                            this->assembleTE->append(QString("R_offset: %1").arg(relTable.r_offset));
                            this->assembleTE->append(QString("R_info: %1\n").arg(relTable.r_info));
                        }

                    break;
                    default:
                        for (int i = 0; i < section64->sh_size; i++){
                            c = buffer[i];
                            /*if (c == '\0') str = " \\0 ";
                            else str = c;*/
                            str = str+c;
                        }
                        this->assembleTE->append(str);
                    break;
                }
            }
            else
            {
                type = section64->sh_type;
                if (type > SHT_NUM)
                {
                    if (nameSection.indexOf(".hash", 0) != -1) type = SHT_HASH;
                    else if (nameSection.indexOf(".rela", 0) != -1) type = SHT_RELA;
                    else if (nameSection.indexOf(".rel", 0) != -1) type = SHT_REL;
                }

                (*file).seekg(section64->sh_offset, ios::beg);
                switch (type)
                {
                    case SHT_SYMTAB:
                        Elf64_Sym symTab;

                        (*file).seekg(section64->sh_offset, ios::beg);

                        while ((*file).tellg() < section64->sh_offset + section64->sh_size)
                        {
                            (*file).read ((char*) &symTab.st_name, sizeof(symTab.st_name));
                            (*file).read ((char*) &symTab.st_value, sizeof(symTab.st_value));
                            (*file).read ((char*) &symTab.st_size, sizeof(symTab.st_size));
                            (*file).read ((char*) &symTab.st_info, sizeof(symTab.st_info));
                            (*file).read ((char*) &symTab.st_other, sizeof(symTab.st_other));
                            (*file).read ((char*) &symTab.st_shndx, sizeof(symTab.st_shndx));

                            this->assembleTE->append(QString("ST_name: %1").arg(symTab.st_name));
                            this->assembleTE->append(QString("ST_value: %1").arg(symTab.st_value));
                            this->assembleTE->append(QString("ST_size: %1").arg(symTab.st_size));
                            this->assembleTE->append(QString("ST_info: %1").arg(symTab.st_info));
                            this->assembleTE->append(QString("ST_other: %1").arg(symTab.st_other));
                            this->assembleTE->append(QString("ST_shndx: %1\n").arg(symTab.st_shndx));
                        }

                    break;
                    case SHT_RELA:
                        Elf64_Rela relaTable;

                        while ((*file).tellg() < section64->sh_offset + section64->sh_size)
                        {
                            (*file).read ((char*) &relaTable.r_offset, sizeof(relaTable.r_offset));
                            (*file).read ((char*) &relaTable.r_info, sizeof(relaTable.r_info));
                            (*file).read ((char*) &relaTable.r_addend, sizeof(relaTable.r_addend));

                            this->assembleTE->append(QString("R_offset: %1").arg(relaTable.r_offset));
                            this->assembleTE->append(QString("R_info: %1").arg(relaTable.r_info));
                            this->assembleTE->append(QString("R_addend: %1\n").arg(relaTable.r_addend));
                        }

                    break;
                    case SHT_HASH:
                        Elf64_Word nbucket;
                        Elf64_Word nchain;
                        Elf64_Word entry;

                        (*file).read ((char*) &nbucket, sizeof(nbucket));
                        (*file).read ((char*) &nchain, sizeof(nchain));

                        this->assembleTE->append(QString("%1").arg(nbucket));
                        this->assembleTE->append(QString("%1").arg(nchain));

                        for (int i = 0; i < nbucket; i++)
                        {
                            (*file).read ((char*) &entry, sizeof(entry));
                            strH = strH + QString("%1\n").arg(entry);
                        }

                        this->assembleTE->append(strH);
                        str = "";

                        for (int i = 0; i < nchain; i++)
                        {
                            (*file).read ((char*) &entry, sizeof(entry));
                            strH = strH + QString("%1\n").arg(entry);
                        }

                        this->assembleTE->append(strH);

                    break;
                    case SHT_DYNAMIC:
                        Elf64_Sxword tag;
                        Elf64_Xword val;
                        Elf64_Addr ptr;

                        while ((*file).tellg() < section64->sh_offset + section64->sh_size)
                        {
                            (*file).read ((char*) &tag, sizeof(tag));
                            this->assembleTE->append(QString("D_TAG: %1").arg(tag));

                            switch(tag)
                            {
                                case 1:
                                case 2:
                                case 8:
                                case 9:
                                case 10:
                                case 11:
                                case 14:
                                case 15:
                                case 18:
                                case 19:
                                case 20:
                                    //d_val
                                    (*file).read ((char*) &val, sizeof(val));
                                    this->assembleTE->append(QString("D_VAL: %1").arg(val));
                                break;

                                case 3:
                                case 4:
                                case 5:
                                case 6:
                                case 7:
                                case 12:
                                case 13:
                                case 17:
                                case 21:
                                case 23:
                                    //d_ptr
                                    (*file).read ((char*) &ptr, sizeof(ptr));
                                    this->assembleTE->append(QString("D_PTR: %1").arg(ptr));
                                break;

                                default:
                                    //do nothing
                                break;
                            }
                            this->assembleTE->append(QString(""));

                        }

                    break;
                    case SHT_NOTE:

                        Elf64_Word name;
                        Elf64_Word descsz;
                        Elf64_Word type;
                        Elf64_Word desc;

                        while ((*file).tellg() < section64->sh_offset + section64->sh_size)
                        {
                            (*file).read ((char*) &name, sizeof(name));
                            (*file).read ((char*) &descsz, sizeof(descsz));
                            (*file).read ((char*) &type, sizeof(type));

                            this->assembleTE->append(QString("Name: %1").arg(name));
                            this->assembleTE->append(QString("DescSZ: %1").arg(descsz));
                            this->assembleTE->append(QString("DescSZ: %1").arg(type));

                            char bufferName [name];
                            (*file).read ((char*) &bufferName, name);

                            str = "";
                            for (int j = 0; j < name; j++)
                            {
                                c = bufferName[j];
                                str = str + c;
                            }

                            this->assembleTE->append(QString(str));

                            if (descsz > 0)
                            {
                                for (int j = 0; j < descsz/4; j++)
                                {
                                    (*file).read ((char*) &desc, sizeof(desc));
                                    this->assembleTE->append(QString("Desc: %1").arg(desc));
                                }
                            }
                        }
                    break;
                    case SHT_REL:
                        Elf64_Rel relTable;

                        while ((*file).tellg() < section64->sh_offset + section64->sh_size)
                        {
                            (*file).read ((char*) &relTable.r_offset, sizeof(relTable.r_offset));
                            (*file).read ((char*) &relTable.r_info, sizeof(relTable.r_info));

                            this->assembleTE->append(QString("R_offset: %1").arg(relTable.r_offset));
                            this->assembleTE->append(QString("R_info: %1\n").arg(relTable.r_info));
                        }
                    break;

                    default:
                        for (int i = 0; i < section64->sh_size; i++){
                            c = buffer[i];
                            /*if (c == '\0') str = " \\0 ";
                            else str = c;*/
                            str = str+c;
                        }
                        this->assembleTE->append(str);
                    break;
                }
            }
        }

        QTextCursor tmpCursor = this->assembleTE->textCursor();
        tmpCursor.movePosition(QTextCursor::Start);
        this->assembleTE->setTextCursor(tmpCursor);

        /*QString text = this->assembleTE->toPlainText();

        this->assembleTE->append("\n\n\n");
        this->assembleTE->append(text);
        if(text.size()>0){
            mychars[0] = text[0].toAscii(); // assuming your text is ascii
        }*/
        /*for (int i = 0; i < section64->sh_size; i++){                              //print
            this->assembleTE->insertPlainText(QString(buffer[i]));
        }*/

    }

    return 0;
}

int MainWindow::setNames()
{
    TElfArchitecture table = elfArch.strTabPtr;
    TElfArchitecture result;
    QString tmp;
    int j;

    if (this->elfArch.strTab > 0)
    {
        fstream file (this->filename.toStdString().c_str(), ios::in|ios::binary|ios::ate);

        char buffer[table->size];
        file.seekg (table->offset, ios::beg);

        file.read ((char*) &buffer, table->size);

        result = elfArch.First;
        for (int i = 0; i < elfArch.count; i++)
        {
            if (result->type == HEADER) tmp = "ELF Header";
            else if (result->type == SEGMENT)
            {
                tmp = getSegType(result->nameIndx);
            }
            else if (result->type == SECTION)
            {
                j = result->nameIndx;
                while (buffer[j] != '\0')
                {
                    tmp = tmp + buffer[j];
                    j++;
                }
            }
            if (tmp == "") tmp = "Empty";
            result->name = new QString (tmp);
            tmp = "";
            result = result->next;
        }
    }
    else return -1;
    return 0;
}

//***********************************Events*************************************************
//******************************************************************************************
int MainWindow::openFile(QString filename)
{
    Elf64_Ehdr header;
    fstream file (filename.toStdString().c_str(), ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        //get filesize
        this->elfArch.filesize = file.tellg();

        //HEADER
        file.seekg (0, ios::beg);   //start of file
        int result = readHeader(&file, &header, CHECK);

        if (result != 0)
        {
            if (result == 1) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.");
            if (result == 2) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.\n Incorrect file class.");
            if (result == 3) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.\n Incorrect file encoding.");
            if (result == 4) QMessageBox::critical(this, "Wrong ELF file", "ELF header of: " + filename + " is not right.\n Incorrect fie version.");
            file.close();
            this->attCh = false;
            this->assCh = false;
            return -1;
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
                if (result != 0)
                {
                    if (result == 1) QMessageBox::critical(this, "Wrong ELF file", "ELF segment has wrong offset!");

                    drawChart();
                    file.close();
                    this->attCh = false;
                    this->assCh = false;
                    return -1;
                }
            }
        }

        //SECTION
        if (header.e_shoff != 0)    //there is program header
        {
            Elf32_Shdr section32;
            Elf64_Shdr section64;
            int result;

            int offset = header.e_shoff;
            file.seekg(offset, ios::beg);

            for (int i = 0; i < header.e_shnum; i++)
            {
                result = readSection(&file, &section32, &section64, header.e_shoff + i * header.e_shentsize, i, CHECK, "");
                if (result != 0)
                {
                    if (result == 1) QMessageBox::critical(this, "Wrong ELF file", "ELF section has wrong offset!");

                    drawChart();
                    file.close();
                    this->attCh = false;
                    this->assCh = false;
                    return -1;
                }
            }
        }

        setNames();
        drawChart();
        this->attCh = false;
        this->assCh = false;
        file.close();
    }
    else
    {
        if (filename != "") QMessageBox::critical(this, "Wrong ELF file", "Can't open file: " + filename + " is not right");
        return -1;
    }
    return 0;
}

void MainWindow::on_actionOpen_File_triggered()
{
    QString filename;

    this->assembleTE->clear();
    this->attributeTE->clear();
    this->gv->clearScene();

    filename = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*)"));
    this->filename = filename;

    openFile(filename);
    return;

 }

int MainWindow::saveFile(QString filenameSave)
{
    if (this->filename != "")
    {
        TElfArchitecture item = seekRecord(this->elfArch.actual);
        Elf64_Ehdr headerForWrite;
        Elf32_Phdr segmentForWrite32;
        Elf64_Phdr segmentForWrite64;
        Elf32_Shdr sectionForWrite32;
        Elf64_Shdr sectionForWrite64;

        char * buffer;
        char* p;
        int size;
        long converted;
        fstream fileIn (this->filename.toStdString().c_str(), ios::in|ios::binary|ios::ate);

        if (fileIn.is_open())
        {
            size = fileIn.tellg();

            fileIn.seekg (0, ios::beg);   //start of file
            buffer = (char*) malloc(sizeof(char) * size);   //alloc
            fileIn.read ((char*) buffer, size);

            fileIn.seekg (0, ios::beg);
            readHeader(&fileIn, &headerForWrite, READ_H);

            fileIn.seekg (item->offsetHeader, ios::beg);

            if (item->type == SEGMENT) readSegment(&fileIn, &segmentForWrite32, &segmentForWrite64, 0, READ_H);
            else readSection(&fileIn, &sectionForWrite32, &sectionForWrite64, 0, 0, READ_H, "");

            fileIn.close();
        }
        else
        {
            QMessageBox::critical(this, "Warning", "Program can't open file: \n" + this->filename);
            return 1;
        }

        ofstream fileOut ((filenameSave).toStdString().c_str(), ios::out|ios::binary|ios::ate);
        if (fileOut.is_open())
        {
            for (int i = 0; i < size; i++)
            {
                if (this->attCh)
                {
                    if (item->offsetHeader == i)
                    {
                        QString str = this->attributeTE->toPlainText();
                        QString substr;
                        int index;

                        if (item->type == HEADER)
                        {
                            //type
                            index = str.indexOf("\n");
                            substr = str.mid(0, index);
                            str.remove(0, index + 1);
                            substr = substr.remove(0, strlen("File type: "));

                            if (substr == "ET_NONE") headerForWrite.e_type = ET_NONE;
                            else if (substr == "ET_REL") headerForWrite.e_type = ET_REL;
                            else if (substr == "ET_EXEC") headerForWrite.e_type = ET_EXEC;
                            else if (substr == "ET_DYN") headerForWrite.e_type = ET_DYN;
                            else if (substr == "ET_CORE") headerForWrite.e_type = ET_CORE;
                            else if (substr == "ET_LOPROC") headerForWrite.e_type = ET_LOPROC;
                            else if (substr == "ET_HIPROC") headerForWrite.e_type = ET_HIPROC;
                            else
                            {
                                converted = strtol(substr.toStdString().c_str(), &p, 10);
                                if (*p) {
                                    //return 2;
                                }
                                else {
                                    headerForWrite.e_type = converted;
                                }
                            }

                            //ph offset
                            index = str.indexOf("Programm header offset: ");
                            str.remove(0, index);

                            index = str.indexOf("\n");
                            substr = str.mid(0, index);
                            str.remove(0, index + 1);
                            substr = substr.remove(0, strlen("Programm header offset: "));

                            converted = strtol(substr.toStdString().c_str(), &p, 10);
                            if (*p) {
                                //return 2;
                            }
                            else {
                                headerForWrite.e_phoff = converted;
                            }

                            //sh offset
                            index = str.indexOf("Section header table offset: ");
                            str.remove(0, index);

                            index = str.indexOf("\n");
                            substr = str.mid(0, index);
                            str.remove(0, index + 1);
                            substr = substr.remove(0, strlen("Section header table offset: "));

                            converted = strtol(substr.toStdString().c_str(), &p, 10);
                            if (*p) {
                                //return 2;
                            }
                            else {
                                headerForWrite.e_shoff = converted;
                            }

                            for (int j = 0; j < EI_NIDENT; j++) fileOut.write((char*) &(headerForWrite.e_ident[j]), sizeof(char));

                            fileOut.write((char *) &(headerForWrite.e_type), sizeof(headerForWrite.e_type));
                            fileOut.write((char *) &(headerForWrite.e_machine), sizeof(headerForWrite.e_machine));
                            fileOut.write((char *) &(headerForWrite.e_version), sizeof(headerForWrite.e_version));
                            fileOut.write((char *) &(headerForWrite.e_entry), sizeof(headerForWrite.e_entry));
                            fileOut.write((char *) &(headerForWrite.e_phoff), sizeof(headerForWrite.e_phoff));
                            fileOut.write((char *) &(headerForWrite.e_shoff), sizeof(headerForWrite.e_shoff));
                            fileOut.write((char *) &(headerForWrite.e_flags), sizeof(headerForWrite.e_flags));
                            fileOut.write((char *) &(headerForWrite.e_ehsize), sizeof(headerForWrite.e_ehsize));
                            fileOut.write((char *) &(headerForWrite.e_phentsize), sizeof(headerForWrite.e_phentsize));
                            fileOut.write((char *) &(headerForWrite.e_phnum), sizeof(headerForWrite.e_phnum));
                            fileOut.write((char *) &(headerForWrite.e_shentsize), sizeof(headerForWrite.e_shentsize));
                            fileOut.write((char *) &(headerForWrite.e_shnum), sizeof(headerForWrite.e_shnum));
                            fileOut.write((char *) &(headerForWrite.e_shstrndx), sizeof(headerForWrite.e_shstrndx));

                            i = i + headerForWrite.e_ehsize;

                        }
                        else if (item->type == SEGMENT)
                        {
                            //Segment offset:
                            index = str.indexOf("Segment offset: ");
                            str.remove(0, index);

                            index = str.indexOf("\n");
                            substr = str.mid(0, index);
                            str.remove(0, index + 1);
                            substr = substr.remove(0, strlen("Segment offset: "));

                            converted = strtol(substr.toStdString().c_str(), &p, 10);
                            if (*p) {
                                //return 2;
                            }
                            else {
                                if (this->elfArch.arch32) segmentForWrite32.p_offset = converted;
                                else segmentForWrite64.p_offset = converted;
                            }

                            if (this->elfArch.arch32)
                            {
                                fileOut.write((char *) &(segmentForWrite32.p_type), sizeof(segmentForWrite32.p_type));
                                fileOut.write((char *) &(segmentForWrite32.p_flags), sizeof(segmentForWrite32.p_flags));
                                fileOut.write((char *) &(segmentForWrite32.p_offset), sizeof(segmentForWrite32.p_offset));
                                fileOut.write((char *) &(segmentForWrite32.p_vaddr), sizeof(segmentForWrite32.p_vaddr));
                                fileOut.write((char *) &(segmentForWrite32.p_paddr), sizeof(segmentForWrite32.p_paddr));
                                fileOut.write((char *) &(segmentForWrite32.p_filesz), sizeof(segmentForWrite32.p_filesz));
                                fileOut.write((char *) &(segmentForWrite32.p_memsz), sizeof(segmentForWrite32.p_memsz));
                                fileOut.write((char *) &(segmentForWrite32.p_align), sizeof(segmentForWrite32.p_align));
                            }
                            else
                            {
                                fileOut.write((char *) &(segmentForWrite64.p_type), sizeof(segmentForWrite64.p_type));
                                fileOut.write((char *) &(segmentForWrite64.p_flags), sizeof(segmentForWrite64.p_flags));
                                fileOut.write((char *) &(segmentForWrite64.p_offset), sizeof(segmentForWrite64.p_offset));
                                fileOut.write((char *) &(segmentForWrite64.p_vaddr), sizeof(segmentForWrite64.p_vaddr));
                                fileOut.write((char *) &(segmentForWrite64.p_paddr), sizeof(segmentForWrite64.p_paddr));
                                fileOut.write((char *) &(segmentForWrite64.p_filesz), sizeof(segmentForWrite64.p_filesz));
                                fileOut.write((char *) &(segmentForWrite64.p_memsz), sizeof(segmentForWrite64.p_memsz));
                                fileOut.write((char *) &(segmentForWrite64.p_align), sizeof(segmentForWrite64.p_align));
                            }
                            i = i + headerForWrite.e_phentsize;
                        }
                        else    //SECTION
                        {
                            //Section offset:
                            index = str.indexOf("Section offset: ");
                            str.remove(0, index);

                            index = str.indexOf("\n");
                            substr = str.mid(0, index);
                            str.remove(0, index + 1);
                            substr = substr.remove(0, strlen("Section offset: "));

                            converted = strtol(substr.toStdString().c_str(), &p, 10);
                            if (*p) {
                                //return 2;
                            }
                            else {
                                if (this->elfArch.arch32) sectionForWrite32.sh_offset = converted;
                                else sectionForWrite64.sh_offset = converted;
                            }

                            if (this->elfArch.arch32)
                            {
                                fileOut.write((char *) &(sectionForWrite32.sh_name), sizeof(sectionForWrite32.sh_name));
                                fileOut.write((char *) &(sectionForWrite32.sh_type), sizeof(sectionForWrite32.sh_type));
                                fileOut.write((char *) &(sectionForWrite32.sh_flags), sizeof(sectionForWrite32.sh_flags));
                                fileOut.write((char *) &(sectionForWrite32.sh_addr), sizeof(sectionForWrite32.sh_addr));
                                fileOut.write((char *) &(sectionForWrite32.sh_offset), sizeof(sectionForWrite32.sh_offset));
                                fileOut.write((char *) &(sectionForWrite32.sh_size), sizeof(sectionForWrite32.sh_size));
                                fileOut.write((char *) &(sectionForWrite32.sh_link), sizeof(sectionForWrite32.sh_link));
                                fileOut.write((char *) &(sectionForWrite32.sh_info), sizeof(sectionForWrite32.sh_info));
                                fileOut.write((char *) &(sectionForWrite32.sh_addralign), sizeof(sectionForWrite32.sh_addralign));
                                fileOut.write((char *) &(sectionForWrite32.sh_entsize), sizeof(sectionForWrite32.sh_entsize));
                            }
                            else
                            {
                                fileOut.write((char *) &(sectionForWrite64.sh_name), sizeof(sectionForWrite64.sh_name));
                                fileOut.write((char *) &(sectionForWrite64.sh_type), sizeof(sectionForWrite64.sh_type));
                                fileOut.write((char *) &(sectionForWrite64.sh_flags), sizeof(sectionForWrite64.sh_flags));
                                fileOut.write((char *) &(sectionForWrite64.sh_addr), sizeof(sectionForWrite64.sh_addr));
                                fileOut.write((char *) &(sectionForWrite64.sh_offset), sizeof(sectionForWrite64.sh_offset));
                                fileOut.write((char *) &(sectionForWrite64.sh_size), sizeof(sectionForWrite64.sh_size));
                                fileOut.write((char *) &(sectionForWrite64.sh_link), sizeof(sectionForWrite64.sh_link));
                                fileOut.write((char *) &(sectionForWrite64.sh_info), sizeof(sectionForWrite64.sh_info));
                                fileOut.write((char *) &(sectionForWrite64.sh_addralign), sizeof(sectionForWrite64.sh_addralign));
                                fileOut.write((char *) &(sectionForWrite64.sh_entsize), sizeof(sectionForWrite64.sh_entsize));
                            }
                            i = i + headerForWrite.e_shentsize;
                        }
                    }
                }

                if (this->assCh)
                {
                    /*QString flagsTmp = readFlags(item->flags);
                    if (flagsTmp.indexOf("Executable", 0) == -1)   //at this moment can't edit assembler
                    {
                        if (item->offset == i)
                        {
                            //get content, write it, i+contentsize before change
                        }
                    }*/
                }

                fileOut.write(&(buffer[i]), sizeof(char));

            }
            fileOut.close();
        }
        else
        {
            QMessageBox::critical(this, "Warning", "Program can't open file: \n" + filenameSave);
            return 2;
        }
    }

    /*if (this->filename != "")
        {
            ifstream fileIn (this->filename.toStdString().c_str(), ios::in|ios::binary|ios::ate);
            ofstream fileOut ((this->filename+"Test").toStdString().c_str(), ios::out|ios::binary|ios::ate);
            if (fileIn.is_open())
            {
                if (fileOut.is_open())
                {
                    int sizeIn = fileIn.tellg();
                    char byte;
                    fileIn.seekg (0, ios::beg);
                    for (int i = 0; i < sizeIn; i++)
                    {
                        fileIn.read(&byte, sizeof(byte));
                        fileOut.write(&byte, sizeof(byte));
                        if (i == 64) //how to corrupt files
                        {
                            byte = 0;
                            fileOut.write(&byte, sizeof(byte));
                        }
                    }
                    fileOut.close();
                }
                fileIn.close();
            }
        }*/
    return 0;
}

void MainWindow::on_actionSave_File_triggered()
{
    saveFile(this->filename);
    openFile(this->filename);
    return;
}

void MainWindow::on_actionSave_As_triggered()
{
    QString saveFilename;
    saveFilename = QFileDialog::getSaveFileName(this, tr("Open Image"), "", tr("Image Files (*)"));
    if (saveFile(saveFilename) > 0) return;
    if (saveFilename == this->filename)
    {
        openFile(saveFilename); //reload
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
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

                if (this->attCh || this->assCh)
                {
                    QMessageBox msgBox;
                    msgBox.setText("The file has been modified.");
                    msgBox.setInformativeText("Do you want to save your changes?");
                    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
                    QAbstractButton* pButtonDis = msgBox.addButton(tr("Don't Save"), QMessageBox::AcceptRole);
                    QAbstractButton* pButtonYes = msgBox.addButton(tr("Save As"), QMessageBox::YesRole);

                    int ret = msgBox.exec();

                    switch(ret)
                    {
                        case QMessageBox::Save: //save
                            if (saveFile(this->filename) > 0) return;
                            openFile(this->filename);
                        break;

                        case QMessageBox::Cancel:   //cancel
                            return;
                        break;

                        default:
                            if (msgBox.clickedButton() == pButtonYes) //save as
                            {
                                QString saveFilename;
                                saveFilename = QFileDialog::getSaveFileName(this, tr("Open Image"), "", tr("Image Files (*)"));
                                if (saveFile(saveFilename) > 0) return;
                                if (saveFilename == this->filename)
                                {
                                    openFile(saveFilename); //reload
                                }
                            }
                            else if (msgBox.clickedButton() == pButtonDis) //don't save
                            {
                                //pass
                            }
                        break;
                    }
                    record = seekRecord(pos);
                }

                this->elfArch.actual = pos;
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
                            case ET_NONE:
                                this->attributeTE->append(QString("File type: ET_NONE"));
                                break;
                            case ET_REL:
                                this->attributeTE->append(QString("File type: ET_REL"));
                                break;
                            case ET_EXEC:
                                this->attributeTE->append(QString("File type: ET_EXEC"));
                                break;
                            case ET_DYN:
                                this->attributeTE->append(QString("File type: ET_DYN"));
                                break;
                            case ET_CORE:
                                this->attributeTE->append(QString("File type: ET_CORE"));
                                break;
                            case ET_LOPROC:
                                this->attributeTE->append(QString("File type: ET_LOPROC"));
                                break;
                            case ET_HIPROC:
                                this->attributeTE->append(QString("File type: ET_HIPROC"));
                                break;
                            default:
                                this->attributeTE->append(QString("File type: %1").arg(header.e_type));
                                break;
                        }

                        if (header.e_ident[4] == 1) this->attributeTE->append(QString("File class: 32-bit"));
                        else this->attributeTE->append(QString("File class: 64-bit"));

                        if (header.e_ident[5] == 1) this->attributeTE->append(QString("File encoding: LSB"));   //LSB
                        else this->attributeTE->append(QString("File encoding: MSB"));   //MSB

                        this->attributeTE->append(QString("OS/ABI: " + getOS(header.e_ident[7])));

                        this->attributeTE->append(QString("Machine: " + getMachines(header.e_machine)));

                        if (header.e_version == 1) this->attributeTE->append(QString("Version: Current version"));
                        else this->attributeTE->append(QString("Version: Invalid version"));

                        this->attributeTE->append(QString("Flags: %1").arg(header.e_flags));
                        this->attributeTE->append(QString("Start entry: %1").arg(header.e_entry));
                        this->attributeTE->append(QString("Header size: %1\n").arg(header.e_ehsize));

                        //if (header.e_phoff != 0)
                        //{
                            this->attributeTE->append(QString("Programm header offset: %1").arg(header.e_phoff));
                            this->attributeTE->append(QString("Number of program header entries: %1").arg(header.e_phnum));
                            this->attributeTE->append(QString("Programm header entry size: %1\n").arg(header.e_phentsize));
                       // }
                        //else this->attributeTE->append(QString("File has no programm header\n"));

                        //if (header.e_shoff != 0)
                        //{
                            this->attributeTE->append(QString("Section header table offset: %1").arg(header.e_shoff));
                            this->attributeTE->append(QString("Number of section header table entries: %1").arg(header.e_shnum));
                            this->attributeTE->append(QString("Section header entry size: %1\n").arg(header.e_shentsize));
                        //}
                        //else this->attributeTE->append(QString("File has no section header table\n"));

                        //if (header.e_shstrndx != 0)
                        //{
                            this->attributeTE->append(QString("Section name string table index: %1").arg(header.e_shstrndx));
                        //}


                        break;

                    case SEGMENT:

                        Elf32_Phdr segment32;
                        Elf64_Phdr segment64;

                        file.seekg(record->offsetHeader, ios::beg);

                        result = readSegment(&file, &segment32, &segment64, 0, READ);
                        if (result == 0)
                        {
                            if (this->elfArch.arch32)
                            {
                                this->attributeTE->append(QString("Segment type: " + getSegType(segment32.p_type)));
                                this->attributeTE->append(QString("Segment offset: %1").arg(segment32.p_offset));
                                this->attributeTE->append(QString("Segment virtual address: %1").arg(segment32.p_vaddr));
                                this->attributeTE->append(QString("Segment physical address: %1").arg(segment32.p_paddr));
                                this->attributeTE->append(QString("Segment size in file: %1").arg(segment32.p_filesz));
                                this->attributeTE->append(QString("Segment size in memory: %1").arg(segment32.p_memsz));
                                QString flags = readFlags(segment32.p_flags);
                                this->attributeTE->append(QString("Segment flags: " + flags));
                                this->attributeTE->append(QString("Segment alignment: %1").arg(segment32.p_align));

                            }
                            else
                            {
                                this->attributeTE->append(QString("Segment type: " + getSegType(segment64.p_type)));
                                this->attributeTE->append(QString("Segment offset: %1").arg(segment64.p_offset));
                                this->attributeTE->append(QString("Segment virtual address: %1").arg(segment64.p_vaddr));
                                this->attributeTE->append(QString("Segment physical address: %1").arg(segment64.p_paddr));
                                this->attributeTE->append(QString("Segment size in file: %1").arg(segment64.p_filesz));
                                this->attributeTE->append(QString("Segment size in memory: %1").arg(segment64.p_memsz));
                                QString flags = readFlags(segment64.p_flags);
                                this->attributeTE->append(QString("Segment flags: " + flags));
                                this->attributeTE->append(QString("Segment alignment: %1").arg(segment64.p_align));
                            }
                        }



                        break;

                    case SECTION:
                        Elf32_Shdr section32;
                        Elf64_Shdr section64;

                        file.seekg(record->offsetHeader, ios::beg);

                        result = readSection(&file, &section32, &section64, 0, -1, READ, *(record->name));
                        if (result == 0)
                        {
                            if (this->elfArch.arch32)
                            {
                                this->attributeTE->append(QString("Section name index: %1").arg(section32.sh_name));
                                this->attributeTE->append(QString("Section type: " + getSecType(section32.sh_type)));
                                QString flags = readFlags(section32.sh_flags);
                                this->attributeTE->append(QString("Section flags: " + flags));
                                this->attributeTE->append(QString("Section address: %1").arg(section32.sh_addr));
                                this->attributeTE->append(QString("Section offset: %1").arg(section32.sh_offset));
                                this->attributeTE->append(QString("Section size: %1").arg(section32.sh_size));
                                this->attributeTE->append(QString("Section link: %1").arg(section32.sh_link));
                                this->attributeTE->append(QString("Section info: %1").arg(section32.sh_info));
                                this->attributeTE->append(QString("Section addess alignment: %1").arg(section32.sh_addralign));
                                this->attributeTE->append(QString("Section entry size: %1").arg(section32.sh_entsize));

                            }
                            else
                            {
                                this->attributeTE->append(QString("Section name index: %1").arg(section64.sh_name));
                                this->attributeTE->append(QString("Section type: " + getSecType(section64.sh_type)));
                                QString flags = readFlags(section64.sh_flags);
                                this->attributeTE->append(QString("Section flags: " + flags));
                                this->attributeTE->append(QString("Section address: %1").arg(section64.sh_addr));
                                this->attributeTE->append(QString("Section offset: %1").arg(section64.sh_offset));
                                this->attributeTE->append(QString("Section size: %1").arg(section64.sh_size));
                                this->attributeTE->append(QString("Section link: %1").arg(section64.sh_link));
                                this->attributeTE->append(QString("Section info: %1").arg(section64.sh_info));
                                this->attributeTE->append(QString("Section addess alignment: %1").arg(section64.sh_addralign));
                                this->attributeTE->append(QString("Section entry size: %1").arg(section64.sh_entsize));
                            }
                        }
                        break;

                    default:
                        break;
                    }
                    this->attCh = false;
                    this->assCh = false;
                    file.close();
                 }

                break;
            }
        }
        pos++;
    }
}

void MainWindow::drawChart()
{
    TElfArchitecture * item;
    item = &elfArch.First;

    this->gv->clearScene();
    emit resizeWindow(this->gv->size().width(), (elfArch.count + 1) * 35 + 30);

    for (int i = 0; i < elfArch.count; i++)
    {
        QString name;
        if ((*item)->show)
        {
            if ((*item)->type == HEADER) name = "ELF Header";
            else if ((*item)->type == SEGMENT)
            {
                this->gv->drawLine(50 + SEGMENT * 20, i * 35 + 23, 40 + SEGMENT * 20, i * 35 + 23);
                this->gv->drawLine(40 + SEGMENT * 20, i * 35 + 23, 40 + SEGMENT * 20, 35);

                if ((*item)->next != NULL && (*item)->next->type == SECTION)
                {
                    this->gv->drawLine(40 + SECTION * 20, i * 35 + 25, 40 + SECTION * 20, i * 35 + 58);
                    this->gv->drawLine(40 + SECTION * 20, i * 35 + 58, 50 + SECTION * 20, i * 35 + 58);
                }
                name = "Segment";
            }
            else
            {
                if (i == 1)
                {
                    this->gv->drawLine(50 + SECTION * 20, i * 35 + 23, 40 + SECTION * 20, i * 35 + 23);
                    this->gv->drawLine(40 + SECTION * 20, i * 35 + 23, 40 + SECTION * 20, 35);
                }
                if ((*item)->next != NULL && (*item)->next->type != SEGMENT)
                {
                    this->gv->drawLine(40 + SECTION * 20, i * 35 + 13, 40 + SECTION * 20, i * 35 + 56);
                    this->gv->drawLine(40 + SECTION * 20, i * 35 + 56, 50 + SECTION * 20, i * 35 + 56);
                }
                name = QString("Section");
            }
            //wrong 50 + type * 20, sectionNumb * 35 + 10, 200, 25
            this->gv->addRectangle((*(*item)->name), (*item)->type, i);
            if ((*item)->failure != 0)
            {
                this->gv->drawLine(50 + (*item)->type * 20, i * 35 + 10, 50 + (*item)->type * 20 + 200, i * 35 + 35);
                this->gv->drawLine(50 + (*item)->type * 20, i * 35 + 35, 50 + (*item)->type * 20 + 200, i * 35 + 10);
            }

        }
        item = &(*item)->next;
    }

}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    emit resizeWindow(this->gv->size().width(), this->gv->size().height());
    drawChart();
}

void MainWindow::attChanged()
{
    this->attCh = true;
}

void MainWindow::assChanged()
{
    this->assCh = true;
}

//******************************************************************************************
//******************************************************************************************

MainWindow::~MainWindow()
{
    delete ui;
}

