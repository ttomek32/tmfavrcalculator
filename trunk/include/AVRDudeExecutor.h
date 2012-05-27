#ifndef AVRDUDEEXECUTOR_H
#define AVRDUDEEXECUTOR_H

#include <QProcess>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QProgressBar>

class AVRDudeExecutor : public QWidget
{
public:
    enum Errors {Err_Ok, Err_FinishingTimeOut, Err_CannotReadSignature, Err_CancelledByUser};   //Lost of possible errors

protected:
    QString ProgrammerType, Port, MCUType, FLASHHex, EEPROMHex;
    bool ShowErrors;                   //True if class should show error messages
    enum Errors LastError;             //Last error that occured

public:
    AVRDudeExecutor(QString aProgrammerType, QString aPort, QString aMCUType, QWidget *parent);
    AVRDudeExecutor(QString aProgrammerType, QString aPort, QString aMCUType, QString aFLASHHex, QString aEEPROMHex, QWidget *parent);
    QString ReadSignature();                   //Odczytaj i zwróæ sygnaturê procesora. Zwraca pusty ³añcuch w razie b³êdu
    QString LookForMCU();                      //Prubuje ustaliæ typ pod³¹czonego procesora, zwraca sygnaturê znalezionego lub pusty ³añcuch jeœli ¿adnego nie znaleziono

    static QStringList *GetAVRDudeCmdMemProgramm(QString aFLASHHex, QString aEEPROMHex, bool verify);   //Zwróæ listê parametrów umo¿liwiaj¹cych programowanie wybranych plików IntelHEX z obrazem FLASH i EEPROM.
                                               //Jeœli oba le¿¹ w tym samym katalogu, to AVRDude zostanie wywo³any z ustawionym katalogiem roboczym,
                                               //tak, aby wskazywa³ na oba pliki.
    bool ProgramMemories(int types, QProgressBar *bar=0);           //Programuj wskazane typy pamiêci, zwraca false w razie b³êdu, opcjonalnie przyjmuje wskaŸnik do paska postêpu

    void SetMCUType(QString aMCUType);         //Zmieñ typ MCU
    void SetExecErr(Errors err) {LastError=err;};    //Typ ostatniego b³êdu
    Errors GetExecErr();
    void SetShowErrors(bool sh) {ShowErrors=sh;};  //W³¹cz/wy³¹cz pokazywanie b³êdóW
    bool GetShowErrors() {return ShowErrors;};
protected:
    QString GetAVRDudeExecPath();                 //Zwróæ œcie¿kê do uruchomienia AVRDude
    void SetBasicAVRDudeParams(QStringList *sl);  //Zwraca listê parametrów wywo³ania AVRDude, zawieraj¹c¹ info o programatorze, porcie i MCU
};

#endif // AVRDUDEEXECUTOR_H
