#ifndef MACHINE_OS_H
#define MACHINE_OS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <QString>
#include <bitset>
#include "elf.h"

QString getMachines(int e_machine);
QString getOS(char os);
QString getSegType(int type);
QString getSecType(int type);

#endif // MACHINE_OS_H
