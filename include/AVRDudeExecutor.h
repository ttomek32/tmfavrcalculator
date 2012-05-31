#ifndef AVRDUDEEXECUTOR_H
#define AVRDUDEEXECUTOR_H

#include <QProcess>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QProgressBar>
//#include <QTextBrowser>
#include "AVRDudeErrorWindow.h"

class AVRDudeExecutor : public QWidget
{
    //Q_OBJECT

public:
    enum Errors {Err_Ok, Err_FinishingTimeOut, Err_CannotReadSignature, Err_CancelledByUser};   //Lost of possible errors

protected:
    QString ProgrammerType, Port, MCUType, FLASHHex, EEPROMHex;
    bool ShowErrors;                   //True if class should show error messages
    enum Errors LastError;             //Last error that occured

public:
    AVRDudeExecutor(QString aProgrammerType, QString aPort, QString aMCUType, QWidget *parent);
    AVRDudeExecutor(QString aProgrammerType, QString aPort, QString aMCUType, QString aFLASHHex, QString aEEPROMHex, QWidget *parent);
    ~AVRDudeExecutor();

    QString ReadSignature();                   //Odczytaj i zwr�� sygnatur� procesora. Zwraca pusty �a�cuch w razie b��du
    QString LookForMCU();                      //Prubuje ustali� typ pod��czonego procesora, zwraca sygnatur� znalezionego lub pusty �a�cuch je�li �adnego nie znaleziono

    static QStringList *GetAVRDudeCmdMemProgramm(QString aFLASHHex, QString aEEPROMHex, bool verify);   //Zwr�� list� parametr�w umo�liwiaj�cych programowanie wybranych plik�w IntelHEX z obrazem FLASH i EEPROM.
                                               //Je�li oba le�� w tym samym katalogu, to AVRDude zostanie wywo�any z ustawionym katalogiem roboczym,
                                               //tak, aby wskazywa� na oba pliki.
    bool ProgramMemories(int types, QProgressBar *bar=0);           //Programuj wskazane typy pami�ci, zwraca false w razie b��du, opcjonalnie przyjmuje wska�nik do paska post�pu

    bool MemoryOp(QStringList type, QString desc); //Odczytaj pami�� o typie okre�lonym przez type (parametr wywo�ania odczytu pami�ci i zapisz wynik do podanego w parametrach pliku
                                               //type musi zawiera� kompletn� lini� wywo�ania AVRDude, desc - opis wy�wietlany na progresie

    void SetMCUType(QString aMCUType);         //Zmie� typ MCU
    void SetExecErr(Errors err) {LastError=err;};    //Typ ostatniego b��du
    Errors GetExecErr();
    void SetShowErrors(bool sh) {ShowErrors=sh;};  //W��cz/wy��cz pokazywanie b��d�W
    bool GetShowErrors() {return ShowErrors;};

protected:
    QString GetAVRDudeExecPath();                 //Zwr�� �cie�k� do uruchomienia AVRDude
    void SetBasicAVRDudeParams(QStringList *sl);  //Zwraca list� parametr�w wywo�ania AVRDude, zawieraj�c� info o programatorze, porcie i MCU
    int ShowAVRDudeOutput();                      //Wy�wietl okienko komunikat�w wyj�ciowych AVRDude
    void AnalyzeOutput(QString output);           //Analizuje komunikaty AVR Dude pod k�tem b��d�w

//private slots:
  //  void AVRDudeOutput();                         //Komunikaty z stdout i stderr trafiaj� do okienka AVRDude
};

#endif // AVRDUDEEXECUTOR_H
