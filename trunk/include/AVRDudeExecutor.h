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
    QString ReadSignature();                   //Odczytaj i zwr�� sygnatur� procesora. Zwraca pusty �a�cuch w razie b��du
    QString LookForMCU();                      //Prubuje ustali� typ pod��czonego procesora, zwraca sygnatur� znalezionego lub pusty �a�cuch je�li �adnego nie znaleziono

    void SetMCUType(QString aMCUType);         //Zmie� typ MCU
protected:
    QString GetAVRDudeExecPath();              //Zwr�� �cie�k� do uruchomienia AVRDude
};

#endif // AVRDUDEEXECUTOR_H
