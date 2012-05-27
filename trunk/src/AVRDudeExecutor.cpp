#include <QSettings>
#include <QStringList>
#include <QFileInfo>
#include <QProgressDialog>
#include <QApplication>

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

void AVRDudeExecutor::SetBasicAVRDudeParams(QStringList *sl)
{
    *sl << "-p";
    *sl << MCUType;        //Dodaj wybrany typ CPU (je�li wybrano)
    *sl << "-c";
    *sl << ProgrammerType; //Dodaj wybrany typ programatora
    *sl << "-P";
    *sl << Port;           //Dodaj wybrany port
}

QString AVRDudeExecutor::ReadSignature()
{
    QString Signature;

    QStringList arguments;
    SetBasicAVRDudeParams(&arguments);   //Uzupe�nij podstawowe parametry wywo�ania AVRDude
    arguments << "-q";                   //Tryb cichszy, mniej informacji do przeparsowania

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
                SetExecErr(Err_Ok);
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

    QProgressDialog progress("Szukam typu MCU...", "&Anuluj", 0, MCUMainTypes.size(), this);
         progress.setWindowModality(Qt::WindowModal);
         progress.setMinimumDuration(0);
         progress.show();

    for(int index=0; index<MCUMainTypes.size(); index++)
    {
        progress.setValue(index);
        if (progress.wasCanceled())
        {
            SetExecErr(Err_CancelledByUser);
            break;
        }
        SetMCUType(MCUMainTypes.at(index));    //Sprawdzaj po kolei r�ne typy r�ni�ce si� sposobem programowania
        signature=ReadSignature();

        Errors tmperr=GetExecErr();
        SetExecErr(tmperr);                //Zachowaj kod b��du
        if(tmperr==Err_FinishingTimeOut)
        {
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

bool AVRDudeExecutor::ProgramMemories(int types, QProgressBar *bar)
{
    bool ret=false;
    int progress=0;   //Warto�� progress bara

    QStringList *arguments=GetAVRDudeCmdMemProgramm(FLASHHex, EEPROMHex, false);
    SetBasicAVRDudeParams(arguments);   //Uzupe�nij podstawowe parametry wywo�ania AVRDude
    //*arguments<<"-t";                   //Programowanie odbywa si� w trybie terminalowym - tak jest wygodniej

    *arguments<<"-n";                   //Nic nie zapisujemy - do test�w

    if(bar) bar->setValue(progress);    //Ustaw progress bar
    QProcess *avrdude = new QProcess(this);
    avrdude->setWorkingDirectory(QFileInfo(FLASHHex).absolutePath());  //Ustawia katalog roboczy, dzi�ki czemu mo�na skr�ci� �cie�ki do plik�w HEX
    avrdude->start(GetAVRDudeExecPath(), *arguments);
    avrdude->setReadChannel(QProcess::StandardError);  //Czytamy z stderr
    //avrdude->setReadChannel(QProcess::StandardOutput);  //Czytamy z stderr
avrdude->waitForFinished(10000);
    //while(!avrdude->waitForFinished(10000))  //Czekaj a� programowanie si� zako�czy
    {
        QApplication::processEvents();

        QString strline;
        while(avrdude->canReadLine())
        {
            strline=strline.append("\n").append(QString::fromLocal8Bit(avrdude->readLine(255)));
        }
        QMessageBox::critical(this, tr("Wyj�cie"), tr("%1").arg(strline), QMessageBox::Ok, QMessageBox::Ok);

        //QString txt;
        //for(int index=0; index<arguments->size(); index++) txt.append(arguments->at(index));
        //QMessageBox::critical(this, tr("Programowanie"),
        //                      tr("%1").arg(txt), QMessageBox::Ok, QMessageBox::Ok);
    }


    return ret;
}

QStringList *AVRDudeExecutor::GetAVRDudeCmdMemProgramm(QString aFLASHHex, QString aEEPROMHex, bool verify)
{
    QString writestring="w";
    if(verify) writestring.append("v");
    QString EEPROMPath=QFileInfo(aEEPROMHex).absolutePath();
    QString FLASHPath=QFileInfo(aFLASHHex).absolutePath();

    QStringList *ret=new QStringList();
    if(aFLASHHex.size()) *ret<<QString("-Uflash:%1:").arg(writestring).append(QFileInfo(aFLASHHex).fileName());

    if(aEEPROMHex.size())
    {
        if(EEPROMPath.compare(FLASHPath))
            { //Oba pliki s� r�nych katalogach
              *ret<<QString("-Ueeprom:%1:").arg(writestring).append(aEEPROMHex); //Dodaj pe�n� �cie�k�
            } else *ret<<QString("-Ueeprom:%1:").arg(writestring).append(QFileInfo(aEEPROMHex).fileName());
    }

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
