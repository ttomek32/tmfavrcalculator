#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QProcess>

#include "qextserialenumerator.h"   //Klasa obs³uguj¹ca porty szeregowe

#include "QProcessErrorMsg.h"
#include "AVRDudeExecutor.h"
#include "maindialog.h"
#include "ui_maindialog.h"
#include "simppgmdlg.h"
#include "ConfigParseException.h"

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    AVRDudeConf(NULL),
    ui(new Ui::MainDialog)
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");
     resize(appsettings.value("size", QSize(640, 480)).toSize());   //restore main window size and position
     move(appsettings.value("pos", QPoint(0, 0)).toPoint());
     appsettings.endGroup();

    ui->setupUi(this);

    QString path;
     appsettings.beginGroup("MainWindow");
     QString ADpath=appsettings.value("AVRDudePath").toString();   //Odczytaj œcie¿kê do AVRDude (jeœli jest)
     emit SetAVRDudePath(ADpath);                          //Uzupe³nij pole edycji œcie¿ki do AVRDude

     path=appsettings.value("FLASHFilePath").toString(); //Odczytaj ostatnio u¿yty plik FLASH
     emit SetFLASHFile(path);                            //Uzupe³nij pola wyboru pliku FLASH we wszystkich zak³adkach

     path=appsettings.value("EEPROMFilePath").toString(); //Odczytaj ostatnio u¿yty plik EEPROM
     emit SetEEPROMFile(path);                            //Uzupe³nij pola wyboru pliku EEPROM we wszystkich zak³adkach

     //Setup tab
     int checkbox;
     checkbox=appsettings.value("SimplifiedView").toInt();
     if(checkbox==Qt::Checked)                           //SprawdŸ czy wybrano widok uproszczony
     {
         HideAdvancedTabs(true);                     //Ukryj zaawansowane zak³adki
         emit SetSimplifierViewChBox(true);
     } else HideAdvancedTabs(false);

     checkbox=appsettings.value("AVRDudeShowWindowOnError").toInt();
     if(checkbox==Qt::Checked)
     {
         emit SetAVRDudeWindowOnError(true);
     };

     checkbox=appsettings.value("AVRDudeShowCMDLine").toInt();
     if(checkbox==Qt::Checked)
     {
         emit SetAVRDudeCMDLine(true);

     } else ui->AVRDudeCMDLineGroupBox->setVisible(false); //Ukryj okno zawieraj¹ce polecenie AVRDude

     ui->VerifyBox->blockSignals(true);
     ui->VerifyBox->setCheckState((Qt::CheckState)appsettings.value("VerifyFLASH", Qt::Unchecked).toInt());   //Odczytaj stan checkboxa weryfikacji zapisu
     ui->VerifyBox->blockSignals(false);

     ui->EraseBox->blockSignals(true);
     ui->EraseBox->setCheckState((Qt::CheckState)appsettings.value("EraseFLASH", Qt::Checked).toInt());     //Odczytaj stan checkboxa kasuj¹cego pamiêæ
     ui->EraseBox->blockSignals(false);

    try
    {
        AVRDudeConf = new AVRDudeConfParser(ADpath+"/avrdude.conf");    //Parser pliku konfiguracyjnego AVRDude

        FillProgrammerCB();                           //Typy obs³ugiwanych programatorów
        QString prg=appsettings.value("Programmer").toString();
        ui->ProgrammerCB->setCurrentIndex(ui->ProgrammerCB->findText(prg));

        FillPortCB();    //Typy dostêpnych portów
        prg=appsettings.value("Port").toString();
        ui->PortCB->setCurrentIndex(ui->PortCB->findText(prg));

        FillMCUType();  //Typy dostêpnych procesorów
        ui->AVRTypeCB->insertItem(0,tr("<Auto>"));
        prg=appsettings.value("MCU").toString();
        ui->AVRTypeCB->setCurrentIndex(ui->AVRTypeCB->findText(prg));
        MCUChanged(prg);  //Ustaw sygnaturê
    }
    catch (ConfigParseException &ex)
    {
         QMessageBox(QMessageBox::Critical, appsettings.value("windowTitle").toString(), QString("Something fucked up!\n\nCo siê sta³o - niebawem.")).exec();
    }

     appsettings.endGroup();
}

void MainDialog::ProgrammBtn()
{
    SimpPgmDialog dlg(this);
    if(dlg.exec()==QMessageBox::Ok)
    {
        ui->SimpProgrammBtn->setText("Przerwij");
        AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), ui->FLASHFile->text(), ui->EEPROMFile->text(), this);
        int res=dlg.GetResult();   //Co wybra³ u¿ytkownik
        AVRDude.ProgramMemories(res, ui->SimpProgrammProgressBar); //Rozpocznij programowanie
        ui->SimpProgrammBtn->setText("Programuj");
    }
}

void MainDialog::EnablePgmButtons()
{
    bool en=true;
    bool FLASHen=true;
    bool EEPROMen=true;

    if(ui->AVRTypeCB->currentText().compare("<Auto>")==0) en=false;  //Nie mo¿emy odblokowaæ jeœli nie wybrano typu MCU
    if(ui->FLASHFile->text().size()==0) FLASHen=false;               //Nie mo¿na programowaæ jeœli nie ma pliku z FLASHEM
    if(ui->EEPROMFile->text().size()==0) EEPROMen=false;             //Lub pliku z EEPROM

    ui->SimpProgrammBtn->setEnabled(en);  //Odblokuj przycisk programowania na zak³adce uproszczonego programowania

    ui->FLASHReadBtn->setEnabled(en && FLASHen);
    ui->FLASHVerifyBtn->setEnabled(en && FLASHen);
    ui->FLASHProgrammBtn->setEnabled(en && FLASHen);

    ui->EEPROMReadBtn->setEnabled(en && EEPROMen);
    ui->EEPROMVerifyBtn->setEnabled(en && EEPROMen);
    ui->EEPROMWriteBtn->setEnabled(en && EEPROMen);

    ui->FuseReadBtn->setEnabled(en);
    ui->FuseVerifyBtn->setEnabled(en);
    ui->FuseWriteBtn->setEnabled(en);

    ui->LockbitReadBtn->setEnabled(en);
    ui->LockbitVerifyBtn->setEnabled(en);
    ui->LockbitWriteBtn->setEnabled(en);
}

void MainDialog::AVRDudeSetPath()
{
    QString path;

    QSettings appsettings;                               //Odczytaj œcie¿kê do AVRDude (jeœli jest)
     appsettings.beginGroup("MainWindow");
     path=appsettings.value("AVRDudePath").toString();
     emit SetAVRDudePath(path);                          //Uzupe³nij pole edycji œcie¿ki do AVRDude

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setDirectory(path);
    QStringList fileNames;
    if (dialog.exec())    //Wyœwietl dialog wyboru œcie¿ki
     {
        fileNames = dialog.selectedFiles();
        if(fileNames.isEmpty()==false)
        {
            path=fileNames.at(0);        //Odczytaj œcie¿kê
            appsettings.setValue("AVRDudePath", path);
            emit SetAVRDudePath(path);
        }
     }

    appsettings.endGroup();  //Zapisz zmiany
}

void MainDialog::OpenFLASHFileDlg()
{
    QString path;

    QSettings appsettings;                               //Odczytaj œcie¿kê do pliku FLASH
     appsettings.beginGroup("MainWindow");
     path=appsettings.value("FLASHFilePath").toString();
     appsettings.endGroup();  //Zapisz zmiany

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("IntelHex (*.hex)"));
    dialog.setDirectory(path);
    QStringList fileNames;
    if (dialog.exec())    //Wyœwietl dialog wyboru œcie¿ki
     {
        fileNames = dialog.selectedFiles();
        if(!fileNames.isEmpty())
        {
            path=fileNames.at(0);        //Odczytaj œcie¿kê
            emit SetFLASHFile(path);     //Uaktualnij pola w których wystêpuje odwo³anie do œcie¿ki pliku hex

            if((ui->AutoEEPBtn->checkState()==Qt::Checked) && (path.endsWith(".hex", Qt::CaseInsensitive)))  //Podano plik o rozszerzeniu .hex, szukamy wiêc czy jest .eep
            {                                                                                                //o ile wybrano opcjê automatyczneg ³adowania EEPROM
                path.truncate(path.size()-4);  //Wycinamy ".hex"
                path.append(".eep");
                if(QFile(path).exists()) emit SetEEPROMFile(path);  //Uaktualnij pola w których znajduje siê œcie¿ka do pliku eep
            }
            AVRDudeCmdLineParams();      //Uaktualnij listê polecenia AVRDude, aby zawiera³a nazwy nowowybranych plików
        }
     }
}

void MainDialog::OpenEEPROMFileDlg()
{
    QString path;

    QSettings appsettings;                               //Odczytaj œcie¿kê do AVRDude (jeœli jest)
     appsettings.beginGroup("MainWindow");
     path=appsettings.value("EEPROMFilePath").toString();
     appsettings.endGroup();

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("IntelHex (*.eep)"));
    dialog.setDirectory(path);
    QStringList fileNames;
    if (dialog.exec())    //Wyœwietl dialog wyboru œcie¿ki
     {
        fileNames = dialog.selectedFiles();
        if(!fileNames.isEmpty())
        {
            path=fileNames.at(0);        //Odczytaj œcie¿kê
            emit SetEEPROMFile(path);                      //Uaktualnij pola w których wystêpuje odwo³anie do œcie¿ki pliku hex
        }
     }
}

void MainDialog::FillProgrammerCB()
{
    ui->ProgrammerCB->clear();   //Skasuj poprzednie pozycje
    if(AVRDudeConf)
    {
        QVector<Programmer> pgm=AVRDudeConf->GetProgrammers();  //Lista programatorów obs³ugiwanych przez AVRDude

        for (int i = 0; i < pgm.size(); ++i)
        {
            ui->ProgrammerCB->addItem(pgm[i].GetID());         //Wype³nij combo z tympami obs³ugiwanych programatorów
        }
    }
}

void MainDialog::FillPortCB()
{
    ui->PortCB->clear();
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();  //Pobierz nazwy dostêpnych portów szeregowych
    for (int i = 0; i < ports.size(); i++)
    {
        ui->PortCB->addItem(ports.at(i).portName);
    }

    ui->PortCB->addItem("usb");
}

void MainDialog::FillMCUType()
{
    ui->AVRTypeCB->clear();   //Skasuj poprzednie pozycje
    if(AVRDudeConf)
    {
        QVector<Part> pgm=AVRDudeConf->GetParts();  //Lista programatorów obs³ugiwanych przez AVRDude

        for (int i = 0; i < pgm.size(); ++i)
        {
            ui->AVRTypeCB->addItem(pgm[i].GetDescription());         //Wype³nij combo z tympami obs³ugiwanych procesorów
        }
    }
}

void MainDialog::TestConnection()
{
    AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), QString(""), QString(""), this);
    QString MCUSig=AVRDude.LookForMCU();
    AVRDudeExecutor::Errors err=AVRDude.GetExecErr();   //Czy wszystko siê skoñczy³o ok?
    if(err==AVRDudeExecutor::Err_Ok)
    {
        ui->AVRSignatureValueLBL->setText(MCUSig);   //Wyœwietl znalezion¹ sygnaturê
        QString MCU=AVRDudeConf->GetPartBySignature(MCUSig).GetDescription();
        ui->AVRTypeCB->setCurrentIndex(ui->AVRTypeCB->findText(MCU));  //ZnajdŸ MCU na podstawie sygnatury
        MCUChanged(MCU);
        ui->ConnectionOk->setText(tr("Po³¹czenie ok"));
    } else
    {
        ui->ConnectionOk->setText(tr("Brak po³¹czenia"));
        QMessageBox::information(this, tr("AVRDude - b³¹d"),
                                 tr("Nie znaleziono sygnatury urz¹dzenia !\nByæ mo¿e nie jest pod³¹czone lub wyst¹pi³ problem z po³¹czeniem."), QMessageBox::Ok, QMessageBox::Ok);
    }

}

void MainDialog::HideAdvancedTabs(bool hide)
{
    for(int i = 1; i < 4; i++)
        ui->Tabs->setTabEnabled(i, !hide);
}

void MainDialog::SetupShowSimplifiedView(int state)
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");
     appsettings.setValue("SimplifiedView", state);   //Zapisz stan przycisku
     appsettings.endGroup();  //Zapisz zmiany

     HideAdvancedTabs(state == Qt::Checked);
}

void MainDialog::SetupShowAVDDudeWindow(int state)
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");
     appsettings.setValue("AVRDudeShowWindowOnError", state);   //Zapisz stan przycisku
     appsettings.endGroup();  //Zapisz zmiany
}

void MainDialog::SetupShowAVRDudeCmd(int state)
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");
     appsettings.setValue("AVRDudeShowCMDLine", state);   //Zapisz stan przycisku
     appsettings.endGroup();  //Zapisz zmiany
     ui->AVRDudeCMDLineGroupBox->setVisible(state==Qt::Checked);  //Ukryj lub poka¿ okienko z lini¹ polecenia AVRDude
}

void MainDialog::SavePathToEEPROMFile(QString file)
{
    QSettings appsettings;                               //Zapisz œcie¿kê do pliku eep
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("EEPROMFilePath", file);
     appsettings.endGroup();  //Zapisz zmiany
}

void MainDialog::SavePathToFLASHFile(QString file)
{
    QSettings appsettings;                               //Zapisz œcie¿kê do pliku eep
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("FLASHFilePath", file);
     appsettings.endGroup();  //Zapisz zmiany
}

 void MainDialog::SavePathToAVRDude(QString path)
 {
     QSettings appsettings;                               //Zapisz œcie¿kê do AVRDude
      appsettings.beginGroup("MainWindow");

      appsettings.setValue("AVRDudePath", path);
      appsettings.endGroup();  //Zapisz zmiany
 }

 void MainDialog::ProgrammerChanged(QString text)
 {
     QSettings appsettings;                               //Zapisz wybrany typ programatora
      appsettings.beginGroup("MainWindow");

      appsettings.setValue("Programmer", text);
      appsettings.endGroup();  //Zapisz zmiany
 }

void MainDialog::PortChanged(QString text)
{
    QSettings appsettings;                               //Zapisz wybrany port
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("Port", text);
     appsettings.endGroup();  //Zapisz zmiany
}

void MainDialog::MCUChanged(QString text)
{
    QString sig=AVRDudeConf->GetPartByDescription(text).GetSignature();  //ZnajdŸ opis wybranego MCU
    ui->AVRSignatureValueLBL->setText(sig);
    ui->StatusLBL->setText(tr("Procesor: ").append(text));   //Uaktualnij liniê stanu wskazuj¹c¹ na typ procesora

    QSettings appsettings;                               //Zapisz wybrany mikrokontroler
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("MCU", text);
     appsettings.endGroup();  //Zapisz zmiany
}

void MainDialog::EraseFLASHChBox(int state)
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("EraseFLASH", state);
     appsettings.endGroup();  //Zapisz zmiany

     AVRDudeCmdLineParams();    //Odœwie¿ okienko stanu
}

void MainDialog::ReadFLASH()
{
    QString Filename=GetFLASHFilePath(); //Nazwa pliku FLASH
    if(QFile(Filename).exists())
    {
        int res=QMessageBox::information(this, tr("Plik"), tr("Plik %1 istnieje. Nadpisaæ go?").arg(Filename), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        switch(res)
        {
        case QMessageBox::Yes: break;
        case QMessageBox::No:  return;   //Nic nie rób
        };
    }
}

void MainDialog::WriteFLASH()
{

}

void MainDialog::VerifyFLASH()
{

}

void MainDialog::ReadEEPROM()
{

}

void MainDialog::WriteEEPROM()
{

}

void MainDialog::VerifyEEPROM()
{

}

void MainDialog::VerifyFLASHChBox(int state)
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("VerifyFLASH", state);
     appsettings.endGroup();  //Zapisz zmiany

     AVRDudeCmdLineParams();    //Odœwie¿ okienko stanu
}

QString MainDialog::GetMCUAsAVRDudeParam()
{
    QString str=AVRDudeConf->GetPartByDescription(ui->AVRTypeCB->currentText()).GetID();
    return str;
}

QString MainDialog::GetProgrammerAsAVRDudeParam()
{
    QString str=ui->ProgrammerCB->currentText();
    return str;
}

QString MainDialog::GetPortAsAVRDudeParam()
{
    QString str=ui->PortCB->currentText();
    return str;
}

QString MainDialog::GetPerformEraseChipAsAVRDudeParam()
{
    QString str="";
    if(ui->EraseBox->checkState()==Qt::Checked) str.append(" -e ");
    return str;
}

void MainDialog::ShowAVRDudeCmdLineParams()
{
    AVRDudeCmdLineParams();
}

void MainDialog::AVRDudeCmdLineParams()
{
    QString str="avrdude";
    str.append(" -p ").append(GetMCUAsAVRDudeParam());
    str.append(" -c ").append(GetProgrammerAsAVRDudeParam());
    str.append(" -P ").append(GetPortAsAVRDudeParam());
    str.append(GetPerformEraseChipAsAVRDudeParam());
    QStringList *sl=AVRDudeExecutor::GetAVRDudeCmdMemProgramm(GetFLASHFilePath(), GetEEPROMFilePath(), ui->VerifyBox->checkState());
    for(int index=0; index<sl->size(); index++) str.append(" ").append(sl->at(index));
    delete sl;
    ui->AVRDudeCMDLine->setText(str);
}

QString MainDialog::GetFLASHFilePath()
{
    return ui->FLASHFile->text();
}

QString MainDialog::GetEEPROMFilePath()
{
    return ui->EEPROMFile->text();
}

MainDialog::~MainDialog()
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");     //Save mainwindow size and position
     appsettings.setValue("size", size());
     appsettings.setValue("pos", pos());
     appsettings.endGroup();

    delete ui;
    delete AVRDudeConf;        //Parser AVRDude ju¿ nam siê nie przyda :)
}
