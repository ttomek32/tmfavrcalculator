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
    SetShowErrors(true);                   //Domyślnie wyświetlamy komunikaty błędów
}

AVRDudeExecutor::~AVRDudeExecutor()
{
    //delete Output;    //Usuń okienko wyjściowe AVRDude
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
    *sl << MCUType;        //Dodaj wybrany typ CPU (jeśli wybrano)
    *sl << "-c";
    *sl << ProgrammerType; //Dodaj wybrany typ programatora
    *sl << "-P";
    *sl << Port;           //Dodaj wybrany port
}

int AVRDudeExecutor::ShowAVRDudeOutput()
{
    int checkbox;

    QSettings appsettings;
     appsettings.beginGroup("MainWindow");
     checkbox=appsettings.value("AVRDudeShowWindowOnError").toInt();
     appsettings.endGroup();

    return checkbox;
}

QString AVRDudeExecutor::ReadSignature()
{
    QString Signature;

    QStringList arguments;
    SetBasicAVRDudeParams(&arguments);   //Uzupełnij podstawowe parametry wywołania AVRDude
    arguments << "-q";                   //Tryb cichszy, mniej informacji do przeparsowania

    QProcess *avrdude = new QProcess(this);
    avrdude->start(GetAVRDudeExecPath(), arguments);
    if(avrdude->waitForFinished(5000)==false)         //Czekaj na koniec, jednak nie więcej niż 5s
    {
        if(GetShowErrors()) QMessageBox::critical(this, tr("AVRDude - błąd"),
                                            tr("Błąd wywołania AVR Dude. Najprawdopodobniej wybrano zły port lub programator."), QMessageBox::Ok, QMessageBox::Ok);
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
        bool hasBeenFound=false;     //Czy znaleziono sygnaturę?

        while(avrdude->canReadLine())
        {
            strline=QString::fromLocal8Bit(avrdude->readLine(255));
            index=strline.indexOf("Device signature = ");           //Czy mamy sygnaturę?
            if(index!=-1)
            {
                strline=strline.right(strline.size()-QString("Device signature = ").size()-index); //Wytnij z całej linii część od sygnatury do końca
                index=0;
                while(strline.at(index)>=QChar('0')) index++; //Usuń z końca linii znaki kontrolne
                Signature=strline.left(index);

                hasBeenFound=true;                          //Sygnaturę znaleziono
                SetExecErr(Err_Ok);
                break;  //Koniec poszukiwań                   //Koniec szukania
            }
        }
        if(hasBeenFound==false)
        {
            if(GetShowErrors()) QMessageBox::critical(this, tr("AVRDude - błąd"),
                                   tr("Nie znaleziono sygnatury urządzenia !\nByć może nie jest podłączone lub wystąpił problem z połączeniem."), QMessageBox::Ok, QMessageBox::Ok);
            SetExecErr(Err_CannotReadSignature);
        }

    } else QProcessErrorMsg(avrdude->error(), this).exec(); //Poinformuj użytkownika o problemie z uruchomieniem AVRDude

    delete avrdude;
    return Signature;     //Zwróć sygnaturę procesora
}

QString AVRDudeExecutor::LookForMCU()
{
    QString signature;
    QString tmpMCU=MCUType;   //Zachowaj aktualny typ MCU
    QStringList MCUMainTypes;
    MCUMainTypes << "t84";       //Lista głównych typów procesorów, różniących się sposobem programowania
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
        SetMCUType(MCUMainTypes.at(index));    //Sprawdzaj po kolei różne typy różniące się sposobem programowania
        signature=ReadSignature();

        Errors tmperr=GetExecErr();
        SetExecErr(tmperr);                //Zachowaj kod błędu
        if(tmperr==Err_FinishingTimeOut)
        {
            if(tmpShowError) QMessageBox::critical(this, tr("AVRDude - błąd"),
                                                tr("Błąd wywołania AVR Dude. Najprawdopodobniej wybrano zły port lub programator."), QMessageBox::Ok, QMessageBox::Ok);
            break;   //Zakończ poszukiwania MCU - będą one bezowocne bo AVRDude nie może się połączyć
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
    bool ShowOutput=ShowAVRDudeOutput();       //Czy wyświetlać wyjście z AVRDude?
    int progress=0;   //Wartość progress bara

    AVRDudeErrorWindow Output(this);
    Output.ui->AVRDudeOutputTxt->clear();      //Skasuj listę wyjściową
    if(ShowOutput)
    {
        Output.setModal(true);      //Okno modalne - blokujemy inne widgety na czas jego wyświetlania
        Output.show();              //Warunkowo wyświetl okienko wyjścia AVRDude
        Output.raise();
    }

    QStringList *arguments=GetAVRDudeCmdMemProgramm(FLASHHex, EEPROMHex, false);
    SetBasicAVRDudeParams(arguments);   //Uzupełnij podstawowe parametry wywołania AVRDude

    if(bar) bar->setValue(progress);    //Ustaw progress bar
    QProcess *avrdude = new QProcess(this);
    avrdude->setWorkingDirectory(QFileInfo(FLASHHex).absolutePath());  //Ustawia katalog roboczy, dzięki czemu można skrócić ścieżki do plików HEX
    avrdude->start(GetAVRDudeExecPath(), *arguments);
    avrdude->setReadChannel(QProcess::StandardError);  //Czytamy z stderr

    while(!avrdude->waitForFinished(10))  //Czekaj aż programowanie się zakończy
    {
        QApplication::processEvents();

        QString strline;
        while(avrdude->canReadLine())
        {
            QByteArray strline = avrdude->readAllStandardError();
            Output.ui->AVRDudeOutputTxt->append(strline);
        }
    }

    while((ShowOutput) && (Output.fin==false))
    {
        QApplication::processEvents();
    }

    if(avrdude->exitCode()) AnalyzeOutput(Output.ui->AVRDudeOutputTxt->toPlainText());  //Wystąpił błąd AVRDude - sprawdź co było przyczyną

    //QMessageBox::critical(this, tr("Wyjście"), tr("Exit code: %1").arg(avrdude->exitCode()), QMessageBox::Ok, QMessageBox::Ok);

    delete avrdude;
    return ret;
}

bool AVRDudeExecutor::MemoryOp(QStringList type, QString desc)
{
    bool ret=false;
    int prg=0;     //Licznik progresu

    int ShowOutput=ShowAVRDudeOutput();       //Czy wy�wietla� wyj�cie z AVRDude?
    AVRDudeErrorWindow Output(this);
    if(ShowOutput==2)
    {
        Output.setModal(true);      //Okno modalne - blokujemy inne widgety na czas jego wy�wietlania
        Output.show();              //Warunkowo wy�wietl okienko wyj�cia AVRDude
        Output.raise();
    }

    QProgressDialog progress(desc, tr("&Anuluj"), 0, 100, this);
         progress.setWindowModality(Qt::WindowModal);
         progress.setMinimumDuration(0);
         progress.show();

    QProcess *avrdude = new QProcess(this);
        avrdude->setWorkingDirectory(QFileInfo(FLASHHex).absolutePath());  //Ustawia katalog roboczy, dzięki czemu można skrócić ścieżki do plików HEX
        avrdude->start(GetAVRDudeExecPath(), type);
        avrdude->setReadChannel(QProcess::StandardError);  //Czytamy z stderr

    while(!avrdude->waitForFinished(100))  //Czekaj a� programowanie si� zako�czy
    {
        QApplication::processEvents();

        QString strline;
        while(avrdude->canReadLine())
        {
            QByteArray strline = avrdude->readAllStandardError();
            Output.ui->AVRDudeOutputTxt->append(strline);
        }
        prg++;
        progress.setValue(prg/10);
        if(progress.wasCanceled()) //U�ytkownik przerwa�� programowanie
        {
            avrdude->kill();  //Zabij AVRDude
            delete avrdude;
            SetExecErr(Err_CancelledByUser);
            return false;
        }
    }

    ret=avrdude->exitCode();   //Sprawdź czy program zakończył się pomyślnie

    if(ret)
    {
        progress.setLabelText(tr("Operacja zakończyła się błędem"));
        if(ShowOutput==1)   //Je�li b��d i u�ytkownik wybra�� opcj� wy�wietlania b��d�w AVRDude
        {
            Output.setModal(true);      //Okno modalne - blokujemy inne widgety na czas jego wy�wietlania
            Output.show();              //Warunkowo wy�wietl okienko wyj�cia AVRDude
            Output.raise();
            AnalyzeOutput(Output.ui->AVRDudeOutputTxt->toPlainText());  //Wyst�pi� b��d AVRDude - sprawd� co by��o przyczyn�
        }
    } else progress.setLabelText(tr("Operacja przebiegła pomyślnie"));

    progress.setAutoClose(false); //Zapobiega autozamykaniu dialogu po osiągnięciu maksimum
    progress.setAutoReset(false);
    progress.setValue(100);
    progress.setCancelButtonText(tr("&Ok"));  //Operacja zakończona, trzeba zmienić Cancel na Ok
    while(!progress.wasCanceled()) QApplication::processEvents();           //Zaczekaj aż użytkownik zamknie dialog

    while(((ShowOutput==2) || ((ShowOutput==1) && ret)) && (Output.fin==false))   //Czekaj na zamknięcie okienka jeśli było ono wcześniej wyświetlone
    {
        QApplication::processEvents();
    }

    delete avrdude;
    return !ret;
}

bool AVRDudeExecutor::ReadByte(QStringList *cmd, uint8_t &byte)
{
    bool ret=false;

    //QMessageBox::critical(this, tr("Polecenie"), tr("%1").arg(cmd->join(QString(" "))), QMessageBox::Ok, QMessageBox::Ok);

    AVRDudeErrorWindow Output(this);

    QProgressDialog progress("Trwa komunikacja...", tr("&Anuluj"), 0, 100, this);
         progress.setWindowModality(Qt::WindowModal);
         progress.setMinimumDuration(2);
         progress.show();

    QProcess *avrdude = new QProcess(this);
        *cmd << "-q";  //quiet mode
        avrdude->start(GetAVRDudeExecPath(), *cmd);
        avrdude->setReadChannel(QProcess::StandardError);  //Czytamy z stderr

    progress.setValue(50);

    while(!avrdude->waitForFinished(100))  //Czekaj a� programowanie si� zako�czy
    {
        QApplication::processEvents();

        QString strline;
        while(avrdude->canReadLine())
        {
            QByteArray strline = avrdude->readAllStandardError();
            Output.ui->AVRDudeOutputTxt->append(strline);
        }
        if(progress.wasCanceled()) //U�ytkownik przerwa�� programowanie
        {
            avrdude->kill();  //Zabij AVRDude
            delete avrdude;
            SetExecErr(Err_CancelledByUser);
            return false;
        }
    }

    ret=avrdude->exitCode();   //Sprawd� czy program zako�czy�� si� pomy�lnie
    if(ret)
    {
        progress.setLabelText(tr("Operacja zakończyła się błędem"));
        AnalyzeOutput(Output.ui->AVRDudeOutputTxt->toPlainText());  //Wyst�pi� b��d AVRDude - sprawd� co by�o przyczyn�
    } else
    {
        if(avrdude->canReadLine())
        {
            bool ok;
            QString tmp=QString("").append(avrdude->readAllStandardOutput());
            byte=tmp.toUInt(&ok, 0);
        }
    }

    delete avrdude;
    return !ret;
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
            { //Oba pliki są różnych katalogach
              *ret<<QString("-Ueeprom:%1:").arg(writestring).append(aEEPROMHex); //Dodaj pełną ścieżkę
            } else *ret<<QString("-Ueeprom:%1:").arg(writestring).append(QFileInfo(aEEPROMHex).fileName());
    }

    return ret;
}

void AVRDudeExecutor::AnalyzeOutput(QString output)
{

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
