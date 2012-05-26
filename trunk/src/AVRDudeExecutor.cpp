#include <QSettings>
#include <QStringList>
#include <QFileInfo>

#include "AVRDudeExecutor.h"
#include "QProcessErrorMsg.h"
#include "delay.h"


AVRDudeExecutor::AVRDudeExecutor(QString aProgrammerType, QString aPort, QString aMCUType, QWidget *parent = 0)
{
    AVRDudeExecutor(aProgrammerType, aPort, aMCUType, QString(""), QString(""), parent);
}

AVRDudeExecutor::AVRDudeExecutor(QString aProgrammerType, QString aPort, QString aMCUType, QString aFLASHHex, QString aEEPROMHex, QWidget *parent) : QWidget(parent)
{
    ProgrammerType=aProgrammerType;
    Port=aPort;
    MCUType=aMCUType;
    FLASHHex=aFLASHHex;
    EEPROMHex=aEEPROMHex;
}

QString AVRDudeExecutor::ReadSignature()
{
    QString Signature;

    QStringList arguments;
    arguments << "-p";
    arguments << MCUType;        //Dodaj wybrany typ CPU (jeœli wybrano)
    arguments << "-c";
    arguments << ProgrammerType; //Dodaj wybrany typ programatora
    arguments << "-P";
    arguments << Port;           //Dodaj wybrany port
    arguments << "-q";                          //Tryb cichszy, mniej informacji do przeparsowania

    QProcess *avrdude = new QProcess(this);
    avrdude->start(GetAVRDudeExecPath(), arguments);
    avrdude->waitForFinished(5000);         //Czekaj na koniec, jednak nie wiêcej ni¿ 5s

    if(avrdude->error()==QProcess::UnknownError)
    {
        avrdude->setReadChannel(QProcess::StandardError);  //Czytamy z stderr

        QString strline;
        int index;
        bool hasBeenFound=false;     //Czy znaleziono sygnaturê?

        while(avrdude->canReadLine())
        {
            strline=QString::fromLocal8Bit(avrdude->readLine(255));
            index=strline.indexOf("Device signature = ");           //Czy mamy sygnaturê?
            if(index!=-1)
            {
                strline=strline.right(strline.size()-QString("Device signature = ").size()-index); //Wytnij z ca³ej linii czêœæ od sygnatury do koñca
                index=0;
                while(strline.at(index)>=QChar('0')) index++; //Usuñ z koñca linii znaki kontrolne
                Signature=strline.left(index);

                hasBeenFound=true;                          //Sygnaturê znaleziono
                break;  //Koniec poszukiwañ                   //Koniec szukania
            }
        }
        if(hasBeenFound==false)
        {
            //QMessageBox::critical(this, tr("AVRDude - b³¹d"),
            //                       tr("Nie znaleziono sygnatury urz¹dzenia !\nByæ mo¿e nie jest pod³¹czone lub wyst¹pi³ problem z po³¹czeniem."), QMessageBox::Ok, QMessageBox::Ok);
            //ui->AVRSignatureValueLBL->setText("brak sygnatury");
        }

    } else QProcessErrorMsg(avrdude->error(), this).exec(); //Poinformuj u¿ytkownika o problemie z uruchomieniem AVRDude

    delete avrdude;
    return Signature;     //Zwróæ sygnaturê procesora
}

QString AVRDudeExecutor::LookForMCU()
{
    QString signature;
    QString tmpMCU=MCUType;   //Zachowaj aktualny typ MCU
    QStringList MCUMainTypes;
    MCUMainTypes << "t84";       //Lista g³ównych typów procesorów, ró¿ni¹cych siê sposobem programowania
    MCUMainTypes << "m8";        //Funkcja po kolei je sprawdza
    MCUMainTypes << "x128a1";
    MCUMainTypes << "2313";
    MCUMainTypes << "usb82";

    for(int index=0; index<MCUMainTypes.size(); index++)
    {
        SetMCUType(MCUMainTypes.at(index));    //Sprawdzaj po kolei ró¿ne typy ró¿ni¹ce siê sposobem programowania
        signature=ReadSignature();
        if(signature.size()) break;
        Delay::sleep(1);
    }

    MCUType=tmpMCU;
    return signature;
}

QStringList *AVRDudeExecutor::GetAVRDudeCmdMemProgramm(QString aFLASHHex, QString aEEPROMHex, bool verify)
{
    QString writestring="w";
    if(verify) writestring.append("v");
    QString EEPROMPath=QFileInfo(aEEPROMHex).absolutePath();
    QString FLASHPath=QFileInfo(aFLASHHex).absolutePath();

    QStringList *ret=new QStringList();
    *ret<<QString("-Uflash:%1:").arg(writestring).append(QFileInfo(aFLASHHex).fileName());

    if(EEPROMPath.compare(FLASHPath))
    { //Oba pliki s¹ ró¿nych katalogach
      *ret<<QString("-Ueeprom:%1:").arg(writestring).append(aEEPROMHex); //Dodaj pe³n¹ œcie¿kê
    } else *ret<<QString("-Ueeprom:%1:").arg(writestring).append(QFileInfo(aEEPROMHex).fileName());

    return ret;
}

void AVRDudeExecutor::SetMCUType(QString aMCUType)
{
    MCUType=aMCUType;
}

QString AVRDudeExecutor::GetAVRDudeExecPath()
{
    QSettings appsettings;
    appsettings.beginGroup("MainWindow");
    QString program = appsettings.value("AVRDudePath").toString()+"/avrdude";
    appsettings.endGroup();
    return program;
}
