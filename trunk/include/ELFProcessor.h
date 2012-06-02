#ifndef ELFPROCESSOR_H
#define ELFPROCESSOR_H

#include <QWidget>
#include <QString>
#include <QByteArray>


//Class realizes access to ELF file. Can extract lock- and fusebytes or generate coresponding Intel HEX files

class ELFProcessor : public QWidget
{
    //Q_OBJECT
    QString pathtoelf;                         //Elf file path

  public:
    ELFProcessor(QString path);                //Open elf file pointed by path

    void GetFuses(QByteArray *);                //Return fusebits from elf
    void GetLocks(QByteArray *);                //Return lockbits from elf
    void CreateFLASHHEXFile();                  //Create FLASH file from elf
    void CreateEEPROMHEXFile();                 //Create eep file from elf

    ~ELFProcessor();
};



#endif // ELFPROCESSOR_H
