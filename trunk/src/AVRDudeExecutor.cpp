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
    SetExecErr(Err_Ok);
    SetShowErrors(true);                   //Domy�lnie wy�wietlamy komunikaty b��d�w
}

AVRDudeExecutor::Errors AVRDudeExecutor::GetExecErr()
{
    Errors tmp=LastError;
    LastError=Err_Ok;
    return tmp;
}

QString AVRDudeExecutor::ReadSignature()
{
    QString Signature;

    QStringList arguments;
    arguments << "-p";
    arguments << MCUType;        //Dodaj wybrany typ CPU (je�li wybrano)
    arguments << "-c";
    arguments << ProgrammerType; //Dodaj wybrany typ programatora
    arguments << "-P";
    arguments << Port;           //Dodaj wybrany port
    arguments << "-q";                          //Tryb cichszy, mniej informacji do przeparsowania

    QProcess *avrdude = new QProcess(this);
    avrdude->start(GetAVRDudeExecPath(), arguments);
    if(avrdude->waitForFinished(5000)==false)         //Czekaj na koniec, jednak nie wi�cej ni� 5s
    {
        if(GetShowErrors()) QMessageBox::critical(this, tr("AVRDude - b��d"),
                                            tr("B��d wywo�ania AVR Dude. Najprawdopodobniej wybrano z�y port lub programator."), QMessageBox::Ok, QMessageBox::Ok);
        avrdude->kill();   //Kille the AVRDude
        delete avrdude;
        SetExecErr(Err_FinishingTimeOut);
        return Signature;
    }

    if(avrdude->error()==QProcess::UnknownError)
    {
        avrdude->setReadChannel(QProcess::StandardError);  //Czytamy z stderr

        QString strline;
        int index;
        bool hasBeenFound=false;     //Czy znaleziono sygnatur�?

        while(avrdude->canReadLine())
        {
            strline=QString::fromLocal8Bit(avrdude->readLine(255));
            index=strline.indexOf("Device signature = ");           //Czy mamy sygnatur�?
            if(index!=-1)
            {
                strline=strline.right(strline.size()-QString("Device signature = ").size()-index); //Wytnij z ca�ej linii cz�� od sygnatury do ko�ca
                index=0;
                while(strline.at(index)>=QChar('0')) index++; //Usu� z ko�ca linii znaki kontrolne
                Signature=strline.left(index);

                hasBeenFound=true;                          //Sygnatur� znaleziono
                break;  //Koniec poszukiwa�                   //Koniec szukania
            }
        }
        if(hasBeenFound==false)
        {
            if(GetShowErrors()) QMessageBox::critical(this, tr("AVRDude - b��d"),
                                   tr("Nie znaleziono sygnatury urz�dzenia !\nBy� mo�e nie jest pod��czone lub wyst�pi� problem z po��czeniem."), QMessageBox::Ok, QMessageBox::Ok);
            SetExecErr(Err_CannotReadSignature);
        }

    } else QProcessErrorMsg(avrdude->error(), this).exec(); //Poinformuj u�ytkownika o problemie z uruchomieniem AVRDude

    delete avrdude;
    return Signature;     //Zwr�� sygnatur� procesora
}

QString AVRDudeExecutor::LookForMCU()
{
    QString signature;
    QString tmpMCU=MCUType;   //Zachowaj aktualny typ MCU
    QStringList MCUMainTypes;
    MCUMainTypes << "t84";       //Lista g��wnych typ�w procesor�w, r�ni�cych si� sposobem programowania
    MCUMainTypes << "m8";        //Funkcja po kolei je sprawdza
    MCUMainTypes << "x128a1";
    MCUMainTypes << "2313";
    MCUMainTypes << "usb82";

    bool tmpShowError=GetShowErrors();
    SetShowErrors(false);

    for(int index=0; index<MCUMainTypes.size(); index++)
    {
        SetMCUType(MCUMainTypes.at(index));    //Sprawdzaj po kolei r�ne typy r�ni�ce si� sposobem programowania
        signature=ReadSignature();

        Errors tmperr=GetExecErr();
        if(tmperr==Err_FinishingTimeOut)
        {
            SetExecErr(tmperr);                //Zachowaj kod b��du
            if(tmpShowError) QMessageBox::critical(this, tr("AVRDude - b��d"),
                                                tr("B��d wywo�ania AVR Dude. Najprawdopodobniej wybrano z�y port lub programator."), QMessageBox::Ok, QMessageBox::Ok);
            break;   //Zako�cz poszukiwania MCU - b�d� one bezowocne bo AVRDude nie mo�e si� po��czy�
        }

        if(signature.size()) break;
        Delay::sleep(1);
    }

    MCUType=tmpMCU;
    SetShowErrors(tmpShowError);
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
    { //Oba pliki s� r�nych katalogach
      *ret<<QString("-Ueeprom:%1:").arg(writestring).append(aEEPROMHex); //Dodaj pe�n� �cie�k�
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
