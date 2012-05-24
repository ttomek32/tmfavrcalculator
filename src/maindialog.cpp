#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QProcess>

#include "qextserialenumerator.h"   //Klasa obs³uguj¹ca porty szeregowe

#include "QProcessErrorMsg.h"
#include "maindialog.h"
#include "ui_maindialog.h"

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
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



     AVRDudeConf=new AvrdudeConfParser(ADpath+"/avrdude.conf");    //Parser pliku konfiguracyjnego AVRDude
     FillProgrammerCB();                           //Typy obs³ugiwanych programatorów
     QString prg=appsettings.value("Programmer").toString();
     ui->ProgrammerCB->setCurrentIndex(ui->ProgrammerCB->findText(prg));

     FillPortCB();    //Typy dostêpnych portów
     prg=appsettings.value("Port").toString();
     ui->PortCB->setCurrentIndex(ui->PortCB->findText(prg));

     FillMCUType();  //Typy dostêpnych procesorów
     prg=appsettings.value("MCU").toString();
     ui->AVRTypeCB->setCurrentIndex(ui->AVRTypeCB->findText(prg));

    appsettings.endGroup();

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
            emit SetFLASHFile(path);                      //Uaktualnij pola w których wystêpuje odwo³anie do œcie¿ki pliku hex

            if(path.endsWith(".hex", Qt::CaseInsensitive))   //Podano plik o rozszerzeniu .hex, szukamy wiêc czy jest .eep
            {
                path.truncate(path.size()-4);  //Wycinamy ".hex"
                path.append(".eep");
                if(QFile(path).exists()) emit SetEEPROMFile(path);  //Uaktualnij pola w których znajduje siê œcie¿ka do pliku eep
            }
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
    QSettings appsettings;
    appsettings.beginGroup("MainWindow");
    QString program = appsettings.value("AVRDudePath").toString()+"/avrdude";
    appsettings.endGroup();

    QStringList arguments;
    //arguments << "-p" << "-c";

    arguments << GetMCUAsAVRDudeParam();        //Dodaj wybrany typ CPU (jeœli wybrano)
    arguments << GetProgrammerAsAVRDudeParam(); //Dodaj wybrany typ programatora
    arguments << GetPortAsAVRDudeParam();       //Dodaj wybrany port

    QProcess *avrdude = new QProcess(this);
    avrdude->start(program, arguments);
    avrdude->waitForFinished(5000);         //Czekaj na koniec, jednak nie wiêcej ni¿ 5s

    //TYLKO DO DEBUGOWANIA
    QString params;
    for(int i=0; i<arguments.size(); i++) params.append(arguments.at(i));

    QMessageBox::warning(this, tr("My Application"),
                         tr("AVR Dude exit code: %1.\n AVRDude cmd: %2\nError: %3, param: %4").arg(avrdude->exitCode()).arg(program).arg(avrdude->error()).arg(params),
                                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

    //A¯ DOT¥D

    if(avrdude->error()==QProcess::UnknownError)
    {
    } else QProcessErrorMsg(avrdude->error(), this).exec(); //Poinformuj u¿ytkownika o problemie z uruchomieniem AVRDude

    delete avrdude;
}

void MainDialog::HideAdvancedTabs(bool hide)
{
    ui->Tabs->setTabEnabled(1, !hide);
    ui->Tabs->setTabEnabled(2, !hide);
    ui->Tabs->setTabEnabled(3, !hide);
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
    QSettings appsettings;                               //Zapisz wybrany mikrokontroler
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("MCU", text);
     appsettings.endGroup();  //Zapisz zmiany
}

QString MainDialog::GetMCUAsAVRDudeParam()
{
    QString str=" -p ";
    str.append(AVRDudeConf->GetPartByDescription(ui->AVRTypeCB->currentText()).GetID());
    return str;
}

QString MainDialog::GetProgrammerAsAVRDudeParam()
{
    QString str=" -c ";
    str.append(ui->ProgrammerCB->currentText());
    return str;
}

QString MainDialog::GetPortAsAVRDudeParam()
{
    QString str=" -P ";
    str.append(ui->PortCB->currentText());
    return str;
}

void MainDialog::ShowAVRDudeCmdLineParams()
{
    QString str="avrdude";
    str.append(GetMCUAsAVRDudeParam());
    str.append(GetProgrammerAsAVRDudeParam());
    str.append(GetPortAsAVRDudeParam());
    ui->AVRDudeCMDLine->setText(str);
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
