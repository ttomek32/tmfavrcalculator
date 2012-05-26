#ifndef AVRDUDEEXECUTOR_H
#define AVRDUDEEXECUTOR_H

#include <QProcess>
#include <QString>
#include <QStringList>
#include <QWidget>

class AVRDudeExecutor : public QWidget
{
protected:
    QString ProgrammerType, Port, MCUType, FLASHHex, EEPROMHex;

public:
    AVRDudeExecutor(QString aProgrammerType, QString aPort, QString aMCUType, QWidget *parent);
    AVRDudeExecutor(QString aProgrammerType, QString aPort, QString aMCUType, QString aFLASHHex, QString aEEPROMHex, QWidget *parent);
    QString ReadSignature();                   //Odczytaj i zwr�� sygnatur� procesora. Zwraca pusty �a�cuch w razie b��du
    QString LookForMCU();                      //Prubuje ustali� typ pod��czonego procesora, zwraca sygnatur� znalezionego lub pusty �a�cuch je�li �adnego nie znaleziono

    static QStringList *GetAVRDudeCmdMemProgramm(QString aFLASHHex, QString aEEPROMHex, bool verify);   //Zwr�� list� parametr�w umo�liwiaj�cych programowanie wybranych plik�w IntelHEX z obrazem FLASH i EEPROM.
                                               //Je�li oba le�� w tym samym katalogu, to AVRDude zostanie wywo�any z ustawionym katalogiem roboczym,
                                               //tak, aby wskazywa� na oba pliki.

    void SetMCUType(QString aMCUType);         //Zmie� typ MCU
protected:
    QString GetAVRDudeExecPath();              //Zwr�� �cie�k� do uruchomienia AVRDude
};

#endif // AVRDUDEEXECUTOR_H
