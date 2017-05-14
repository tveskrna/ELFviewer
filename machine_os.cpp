#include "machine_os.h"

QString getSegType(int type)
{
    switch (type)
    {
        case PT_NULL:
            return "PT_NULL";
        break;

        case PT_LOAD :
            return "PT_LOAD";
        break;

        case PT_DYNAMIC :
            return "PT_DYNAMIC";
        break;

        case PT_INTERP :
            return "PT_INTERP";
        break;

        case PT_NOTE :
            return "PT_NOTE";
        break;

        case PT_SHLIB :
            return "PT_SHLIB";
        break;

        case PT_PHDR :
            return "PT_PHDR";
        break;

        case PT_LOPROC :
            return "PT_LOPROC";
        break;

        case PT_HIPROC :
            return "PT_HIPROC";
        break;

        case PT_TLS:
            return "PT_TLS";
        break;

        case PT_LOOS:
            return "PT_LOOS";
        break;

        case PT_HIOS:
            return "PT_HIOS";
        break;

        default:
            return "Procesor specific";
        break;
    }
}

QString getSecType(int type)
{
    switch (type) {
        case 0: return "SHT_NULL";
        case 1: return "SHT_PROGBITS";
        case 2: return "SHT_SYMTAB";
        case 3: return "SHT_STRTAB";
        case 4: return "SHT_RELA";
        case 5: return "SHT_HASH";
        case 6: return "SHT_DYNAMIC";
        case 7: return "SHT_NOTE";
        case 8: return "SHT_NOBITS";
        case 9: return "SHT_REL";
        case 10: return "SHT_SHLIB";
        case 11: return "SHT_DYNSYM";
        default:
            return "Procesor specific";
        break;
    }
}

QString getOS(char os)
{
    switch (os)
    {
        case 0: return "System V";
        case 1: return "HP-UX";
        case 2: return "NetBSD";
        case 3: return "Linux";
        case 4: return "GNU Hurd";
        case 6: return "Solaris";
        case 7: return "AIX";
        case 8: return "IRIX";
        case 9: return "FreeBSD";
        case 10: return "Tru64";
        case 11: return "Novell Modesto";
        case 12: return "OpenBSD";
        case 13: return "OpenVMS";
        case 14: return "NonStop Kernel";
        case 15: return "AROS";
        case 16: return "Fenix OS";
        case 17: return "CloudABI";
        case 83: return "Sortix";
        default:
            return "Unspecific OS";
        break;
    }
}

QString getMachines(int e_machine)
{
    switch (e_machine)
    {
        case 1: return "AT&T WE 32100";
        case 2: return "SPARC";
        case 3: return "Intel 80386";
        case 4: return "Motorola 68000";
        case 5: return "Motorola 88000";
        case 6: return "Intel MCU";
        case 7: return "Intel 80860";
        case 8: return "MIPS I Architecture";
        case 9: return "IBM System/370 Processor";
        case 10: return "MIPS RS3000 Little-endian";
        case 15: return "Hewlett-Packard PA-RISC";
        case 17: return "Fujitsu VPP500";
        case 18: return "Enhanced instruction set SPARC";
        case 19: return "Intel 80960";
        case 20: return "PowerPC";
        case 21: return "64-bit PowerPC";
        case 22: return "IBM System/390 Processor";
        case 23: return "IBM SPU/SPC";
        case 36: return "NEC V800";
        case 37: return "Fujitsu FR20";
        case 38: return "TRW RH-32";
        case 39: return "Motorola RCE";
        case 40: return "ARM 32-bit architecture (AARCH32)";
        case 41: return "Digital Alpha";
        case 42: return "Hitachi SH";
        case 43: return "SPARC Version 9";
        case 44: return "Siemens TriCore embedded processor";
        case 45: return "Argonaut RISC Core, Argonaut Technologies Inc.";
        case 46: return "Hitachi H8/300";
        case 47: return "Hitachi H8/300H";
        case 48: return "Hitachi H8S";
        case 49: return "Hitachi H8/500";
        case 50: return "Intel IA-64 processor architecture";
        case 51: return "Stanford MIPS-X";
        case 52: return "Motorola ColdFire";
        case 53: return "Motorola M68HC12";
        case 54: return "Fujitsu MMA Multimedia Accelerator";
        case 55: return "Siemens PCP";
        case 56: return "Sony nCPU embedded RISC processor";
        case 57: return "Denso NDR1 microprocessor";
        case 58: return "Motorola Star*Core processor";
        case 59: return "Toyota ME16 processor";
        case 60: return "STMicroelectronics ST100 processor";
        case 61: return "Advanced Logic Corp. TinyJ embedded processor family";
        case 62: return "AMD x86-64 architecture";
        case 63: return "Sony DSP Processor";
        case 64: return "Digital Equipment Corp. PDP-10";
        case 65: return "Digital Equipment Corp. PDP-11";
        case 66: return "Siemens FX66 microcontroller";
        case 67: return "STMicroelectronics ST9+ 8/16 bit microcontroller";
        case 68: return "STMicroelectronics ST7 8-bit microcontroller";
        case 69: return "Motorola MC68HC16 Microcontroller";
        case 70: return "Motorola MC68HC11 Microcontroller";
        case 71: return "Motorola MC68HC08 Microcontroller";
        case 72: return "Motorola MC68HC05 Microcontroller";
        case 73: return "Silicon Graphics SVx";
        case 74: return "STMicroelectronics ST19 8-bit microcontroller";
        case 75: return "Digital VAX";
        case 76: return "Axis Communications 32-bit embedded processor";
        case 77: return "Infineon Technologies 32-bit embedded processor";
        case 78: return "Element 14 64-bit DSP Processor";
        case 79: return "LSI Logic 16-bit DSP Processor";
        case 80: return "Donald Knuth's educational 64-bit processor";
        case 81: return "Harvard University machine-independent object files";
        case 82: return "SiTera Prism";
        case 83: return "Atmel AVR 8-bit microcontroller";
        case 84: return "Fujitsu FR30";
        case 85: return "Mitsubishi D10V";
        case 86: return "Mitsubishi D30V";
        case 87: return "NEC v850";
        case 88: return "Mitsubishi M32R";
        case 89: return "Matsushita MN10300";
        case 90: return "Matsushita MN10200";
        case 91: return "picoJava";
        case 92: return "OpenRISC 32-bit embedded processor";
        case 93: return "ARC International ARCompact processor (old spelling/synonym: EM_ARC_A5)";
        case 94: return "Tensilica Xtensa Architecture";
        case 95: return "Alphamosaic VideoCore processor";
        case 96: return "Thompson Multimedia General Purpose Processor";
        case 97: return "National Semiconductor 32000 series";
        case 98: return "Tenor Network TPC processor";
        case 99: return "Trebia SNP 1000 processor";
        case 100: return "STMicroelectronics (www.st.com) ST200 microcontroller";
        case 101: return "Ubicom IP2xxx microcontroller family";
        case 102: return "MAX Processor";
        case 103: return "National Semiconductor CompactRISC microprocessor";
        case 104: return "Fujitsu F2MC16";
        case 105: return "Texas Instruments embedded microcontroller msp430";
        case 106: return "Analog Devices Blackfin (DSP) processor";
        case 107: return "S1C33 Family of Seiko Epson processors";
        case 108: return "Sharp embedded microprocessor";
        case 109: return "Arca RISC Microprocessor";
        case 110: return "Microprocessor series from PKU-Unity Ltd. and MPRC of Peking University";
        case 111: return "eXcess: 16/32/64-bit configurable embedded CPU";
        case 112: return "Icera Semiconductor Inc. Deep Execution Processor";
        case 113: return "Altera Nios II soft-core processor";
        case 114: return "National Semiconductor CompactRISC CRX microprocessor";
        case 115: return "Motorola XGATE embedded processor";
        case 116: return "Infineon C16x/XC16x processor";
        case 117: return "Renesas M16C series microprocessors";
        case 118: return "Microchip Technology dsPIC30F Digital Signal Controller";
        case 119: return "Freescale Communication Engine RISC core";
        case 120: return "Renesas M32C series microprocessors";
        case 131: return "Altium TSK3000 core";
        case 132: return "Freescale RS08 embedded processor";
        case 133: return "Analog Devices SHARC family of 32-bit DSP processors";
        case 134: return "Cyan Technology eCOG2 microprocessor";
        case 135: return "Sunplus S+core7 RISC processor";
        case 136: return "New Japan Radio (NJR) 24-bit DSP Processor";
        case 137: return "Broadcom VideoCore III processor";
        case 138: return "RISC processor for Lattice FPGA architecture";
        case 139: return "Seiko Epson C17 family";
        case 140: return "The Texas Instruments TMS320C6000 DSP family";
        case 141: return "The Texas Instruments TMS320C2000 DSP family";
        case 142: return "The Texas Instruments TMS320C55x DSP family";
        case 143: return "Texas Instruments Application Specific RISC Processor, 32bit fetch";
        case 144: return "Texas Instruments Programmable Realtime Unit";
        case 160: return "STMicroelectronics 64bit VLIW Data Signal Processor";
        case 161: return "Cypress M8C microprocessor";
        case 162: return "Renesas R32C series microprocessors";
        case 163: return "NXP Semiconductors TriMedia architecture family";
        case 164: return "QUALCOMM DSP6 Processor";
        case 165: return "Intel 8051 and variants";
        case 166: return "STMicroelectronics STxP7x family of configurable and extensible RISC processors";
        case 167: return "Andes Technology compact code size embedded RISC processor family";
        case 168: return "Cyan Technology eCOG1X family";
        case 169: return "Dallas Semiconductor MAXQ30 Core Micro-controllers";
        case 170: return "New Japan Radio (NJR) 16-bit DSP Processor";
        case 171: return "M2000 Reconfigurable RISC Microprocessor";
        case 172: return "Cray Inc. NV2 vector architecture";
        case 173: return "Renesas RX family";
        case 174: return "Imagination Technologies META processor architecture";
        case 175: return "MCST Elbrus general purpose hardware architecture";
        case 176: return "Cyan Technology eCOG16 family";
        case 177: return "National Semiconductor CompactRISC CR16 16-bit microprocessor";
        case 178: return "Freescale Extended Time Processing Unit";
        case 179: return "Infineon Technologies SLE9X core";
        case 180: return "Intel L10M";
        case 181: return "Intel K10M";
        case 183: return "ARM 64-bit architecture (AARCH64)";
        case 185: return "Atmel Corporation 32-bit microprocessor family";
        case 186: return "STMicroeletronics STM8 8-bit microcontroller";
        case 187: return "Tilera TILE64 multicore architecture family";
        case 188: return "Tilera TILEPro multicore architecture family";
        case 189: return "Xilinx MicroBlaze 32-bit RISC soft processor core";
        case 190: return "NVIDIA CUDA architecture";
        case 191: return "Tilera TILE-Gx multicore architecture family";
        case 192: return "CloudShield architecture family";
        case 193: return "KIPO-KAIST Core-A 1st generation processor family";
        case 194: return "KIPO-KAIST Core-A 2nd generation processor family";
        case 195: return "Synopsys ARCompact V2";
        case 196: return "Open8 8-bit RISC soft processor core";
        case 197: return "Renesas RL78 family";
        case 198: return "Broadcom VideoCore V processor";
        case 199: return "Renesas 78KOR family";
        case 200: return "Freescale 56800EX Digital Signal Controller (DSC)";
        case 201: return "Beyond BA1 CPU architecture";
        case 202: return "Beyond BA2 CPU architecture";
        case 203: return "XMOS xCORE processor family";
        case 204: return "Microchip 8-bit PIC(r) family";
        case 205: return "Reserved by Intel";
        case 206: return "Reserved by Intel";
        case 207: return "Reserved by Intel";
        case 208: return "Reserved by Intel";
        case 209: return "Reserved by Intel	";
        case 210: return "KM211 KM32 32-bit processor";
        case 211: return "KM211 KMX32 32-bit processor";
        case 212: return "KM211 KMX16 16-bit processor";
        case 213: return "KM211 KMX8 8-bit processor";
        case 214: return "KM211 KVARC processor";
        case 215: return "Paneve CDP architecture family";
        case 216: return "Cognitive Smart Memory Processor";
        case 217: return "Bluechip Systems CoolEngine";
        case 218: return "Nanoradio Optimized RISC";
        case 219: return "CSR Kalimba architecture family";
        case 220: return "Zilog Z80";
        case 221: return "Controls and Data Services VISIUMcore processor";
        case 222: return "FTDI Chip FT32 high performance 32-bit RISC architecture";
        case 223: return "Moxie processor family";
        case 224: return "AMD GPU architecture	";
        case 243: return "RISC-V";
        default:
            return "Procesor specific";
        break;
    }
}
