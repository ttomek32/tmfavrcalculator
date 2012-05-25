#ifndef AVRDUDEEXECUTOR_H
#define AVRDUDEEXECUTOR_H

#include <QProcess>
#include <QString>
#include <QWidget>

class AVRDudeExecutor : public QWidget
{
protected:
    QString ProgrammerType, Port, MCUType;

public:
    AVRDudeExecutor(QString aProgrammerType, QString aPort, QString aMCUType, QWidget *parent = 0);
    QString ReadSignature();                   //Odczytaj i zwróæ sygnaturê procesora. Zwraca pusty ³añcuch w razie b³êdu
    QString LookForMCU();                      //Prubuje ustaliæ typ pod³¹czonego procesora, zwraca sygnaturê znalezionego lub pusty ³añcuch jeœli ¿adnego nie znaleziono

    void SetMCUType(QString aMCUType);         //Zmieñ typ MCU
protected:
    QString GetAVRDudeExecPath();              //Zwróæ œcie¿kê do uruchomienia AVRDude
};

#endif // AVRDUDEEXECUTOR_H
