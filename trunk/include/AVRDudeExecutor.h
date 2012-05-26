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
    QString ReadSignature();                   //Odczytaj i zwróæ sygnaturê procesora. Zwraca pusty ³añcuch w razie b³êdu
    QString LookForMCU();                      //Prubuje ustaliæ typ pod³¹czonego procesora, zwraca sygnaturê znalezionego lub pusty ³añcuch jeœli ¿adnego nie znaleziono

    static QStringList *GetAVRDudeCmdMemProgramm(QString aFLASHHex, QString aEEPROMHex, bool verify);   //Zwróæ listê parametrów umo¿liwiaj¹cych programowanie wybranych plików IntelHEX z obrazem FLASH i EEPROM.
                                               //Jeœli oba le¿¹ w tym samym katalogu, to AVRDude zostanie wywo³any z ustawionym katalogiem roboczym,
                                               //tak, aby wskazywa³ na oba pliki.

    void SetMCUType(QString aMCUType);         //Zmieñ typ MCU
protected:
    QString GetAVRDudeExecPath();              //Zwróæ œcie¿kê do uruchomienia AVRDude
};

#endif // AVRDUDEEXECUTOR_H
