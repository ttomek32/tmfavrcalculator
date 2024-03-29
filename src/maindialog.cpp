#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QCheckBox>
#include <QComboBox>
#include <QVariant>
#include <QMap>

#include "qextserialenumerator.h"   //Klasa obsługująca porty szeregowe

#include "QProcessErrorMsg.h"
#include "AVRDudeExecutor.h"
#include "maindialog.h"
#include "ui_maindialog.h"
#include "simppgmdlg.h"
#include "ConfigParseException.h"

#include "splash.h"

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::MainDialog)
{
    AVR=NULL;
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");
     resize(appsettings.value("size", QSize(640, 480)).toSize());   //restore main window size and position
     move(appsettings.value("pos", QPoint(0, 0)).toPoint());
     appsettings.endGroup();

    ui->setupUi(this);

    QString path;
     appsettings.beginGroup("MainWindow");
     QString ADpath=appsettings.value("AVRDudePath").toString();   //Odczytaj �cie�k� do AVRDude (je�li jest)
     emit SetAVRDudePath(ADpath);                          //Uzupe�nij pole edycji �cie�ki do AVRDude

     ADpath=appsettings.value("AVRBinutilsPath").toString();   //Odczytaj �cie�k� do binutils (je�lii jest)
     emit SetBinutilsPath(ADpath);

     path=appsettings.value("FLASHFilePath").toString(); //Odczytaj ostatnio użyty plik FLASH
     emit SetFLASHFile(path);                            //Uzupełnij pola wyboru pliku FLASH we wszystkich zakładkach

     path=appsettings.value("EEPROMFilePath").toString(); //Odczytaj ostatnio użyty plik EEPROM
     emit SetEEPROMFile(path);                            //Uzupełnij pola wyboru pliku EEPROM we wszystkich zakładkach

     //Setup tab
     int checkbox;
     checkbox=appsettings.value("SimplifiedView").toInt();
     if(checkbox==Qt::Checked)                           //Sprawd� czy wybrano widok uproszczony
     {
         HideAdvancedTabs(true);                     //Ukryj zaawansowane zak�adki
         emit SetSimplifierViewChBox(true);
     } else HideAdvancedTabs(false);

     checkbox=appsettings.value("AVRDudeShowWindowOnError").toInt();
     ui->ShowAVRDudeWindow->setCurrentIndex(checkbox);

     checkbox=appsettings.value("AVRDudeShowCMDLine").toInt();
     if(checkbox==Qt::Checked)
     {
         emit SetAVRDudeCMDLine(true);

     } else ui->AVRDudeCMDLineGroupBox->setVisible(false); //Ukryj okno zawierające polecenie AVRDude

     ui->VerifyBox->blockSignals(true);
     ui->VerifyBox->setCheckState((Qt::CheckState)appsettings.value("VerifyFLASH", Qt::Unchecked).toInt());   //Odczytaj stan checkboxa weryfikacji zapisu
     ui->VerifyBox->blockSignals(false);

     ui->EraseBox->blockSignals(true);
     ui->EraseBox->setCheckState((Qt::CheckState)appsettings.value("EraseFLASH", Qt::Checked).toInt());     //Odczytaj stan checkboxa kasującego pamięć
     ui->EraseBox->blockSignals(false);

     QList<QLineEdit*> tle = findChildren<QLineEdit*>(LastSelFuseByte);  //Zapisz oryginaln� palet� aktywnego okna edycji fusebit�w
     editpal=tle.at(0)->palette();

     ReLoadAVRDudeConf();

     appsettings.endGroup();

     LastSelFuseByte=QString("Fuse0");   //Nazwa domy�lnie wybranego fusebajtu
     FuseByteChanged();
}

bool MainDialog::ReLoadAVRDudeConf()
{
    bool ret=true;
    QSettings appsettings;

    appsettings.beginGroup("MainWindow");

    QString ADpath=appsettings.value("AVRDudePath").toString();   //Odczytaj �cie�k� do AVRDude (je�li jest)
    QFileInfo fi(ADpath, "avrdude.conf");
    if(fi.exists())
    {
        if(AVR) delete AVR;
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        AVR= new AVRFactory(fi.filePath(), "../XML/");   //Tymczasowo
        QApplication::restoreOverrideCursor();

        DisableTabs(false);                           //�cie�ka do AVRDude prawid�owa, mo�na odblokowa� zak�adki
        ui->ProgrammerCB->blockSignals(true);
        FillProgrammerCB();                           //Typy obs�ugiwanych programator�w
        ui->ProgrammerCB->blockSignals(false);
        QString prg=appsettings.value("Programmer").toString();
        ui->ProgrammerCB->setCurrentIndex(ui->ProgrammerCB->findText(prg));

        ui->PortCB->blockSignals(true);
        FillPortCB();    //Typy dost�pnych port�w
        ui->PortCB->blockSignals(false);
        prg=appsettings.value("Port").toString();
        ui->PortCB->setCurrentIndex(ui->PortCB->findText(prg));

        ui->AVRTypeCB->blockSignals(true);
        FillMCUType();  //Typy dost�pnych procesor�w
        ui->AVRTypeCB->insertItem(0,tr("<Auto>"));
        prg=appsettings.value("MCU").toString();
        ui->AVRTypeCB->blockSignals(false);
        ui->AVRTypeCB->setCurrentIndex(ui->AVRTypeCB->findText(prg));
        MCUChanged(prg);  //Ustaw sygnatur�

    } else
    {
        ret=false;
        DisableTabs(true);           //Wy��cz wszystkie zak�adki z wyj�tkiem SetUp
        if(splash) splash->hide();   //Ukryj splash screen je�li jest, �eby poni�szy komunikat nie by� pod nim
        QMessageBox(QMessageBox::Critical, appsettings.value("windowTitle").toString(), QString("Nie da si� otworzy� pliku konfiguracyjnego AVRDude.conf.")).exec();
    }

    appsettings.endGroup();
    return ret;
}

void MainDialog::UpdateFuseBitsWidget()
{
    Part mcu=AVR->GetPartByDescription(ui->AVRTypeCB->currentText());
    QVector<Bit> fuses=mcu.GetFuseBits();
    FillTableWidget(fuses, ui->FusebitTable, SLOT(FuseBitChangedByUser()));
    FuseByteChanged();   //Uaktualnij stan tabeli na podstawie bajtowej warto�ci fusebit�w
}

void MainDialog::UpdateLockBitsWidget()
{
    Part mcu=AVR->GetPartByDescription(ui->AVRTypeCB->currentText());
    QVector<Bit> locks=mcu.GetLockBits();
    FillTableWidget(locks, ui->LockbitTable, SLOT(LockBitChangedByUser()));
    LockByteChanged();   //Uaktualnij stan tabeli na podstawie bajtowej warto�ci lockbit�w
}

void MainDialog::FillTableWidget(QVector<Bit> &desc, QTableWidget *tbl, const char* slot)
{
    for (int i=tbl->rowCount()-1; i >= 0; --i) tbl->removeRow(i);  //Skasuj wszystkie rz�dy
    for(int i=0; i<desc.size(); i++)
    {
        tbl->setRowCount(tbl->rowCount()+1);
        if(desc[i].GetValues().size()==0)
        { //Wstawiamy checkboxy
            QCheckBox *cb=new QCheckBox;
            cb->setText(desc[i].GetName());    //Nazwa skr�cona fuse/lock bitu
            tbl->setCellWidget(i, 0, cb);
            connect(cb, SIGNAL(clicked()), this, slot);

        } else
        { //Pole wielokrotnego wyboru - comboboxy
            QComboBox *cb=new QComboBox;
            tbl->setCellWidget(i, 0, cb);
            for(int ind=0; ind<desc[i].GetValues().size(); ind++)
            {
                cb->addItem(desc[i].GetValues()[ind].GetName());
            }
            connect(cb, SIGNAL(currentIndexChanged(int)), this, slot);
        }
        QLabel *lab=new QLabel;
        lab->setText(desc[i].GetCaption());        //Opis szczeg�owy fuse/lock bitu
        tbl->setCellWidget(i,1,lab);   //Wstaw opis szczeg�owy fuse/lock bitu
    }
}

void MainDialog::FuseBitChangedByUser()
{
    bool ok;
    uint8_t fuse[5]={0xff, 0xff, 0xff, 0xff, 0xff};     //Unused fusebits are always unprogrammed
    Part mcu=AVR->GetPartByDescription(ui->AVRTypeCB->currentText());
    QVector<Bit> fuses=mcu.GetFuseBits();
    for(int i=0; i<ui->FusebitTable->rowCount(); i++)   //Scan all rows and set fusebits appropiately
    {
        uint8_t mask=fuses[i].GetMask().toUInt(&ok, 0);      //Get bitmask of subsequent fields
        uint8_t foffset=fuses[i].GetOffset().toUInt(&ok, 0); //Get fusebyte offset
        QWidget *tb=ui->FusebitTable->cellWidget(i, 0); //Get widget at indicated row
        if(dynamic_cast<QComboBox*>(tb))
        {
            QString str=((QComboBox*)tb)->currentText();   //Pobierz wybrane ustwienie
            fuse[foffset]&=~mask;                          //Erase masked bits
            uint8_t offs=0;
            while((mask & (1<<offs))==0) offs++;  //Calculate needed rotation of value field
            for(int ind=0; ind<fuses[i].GetValues().size(); ind++)
                if(fuses[i].GetValues()[ind].GetName().compare(str)==0)
                       fuse[foffset]|=(fuses[i].GetValues()[ind].GetValue().toUInt(&ok,0) << offs);
        } else if(dynamic_cast<QCheckBox*>(tb)) if(((QCheckBox*)tb)->checkState()==Qt::Checked) fuse[foffset]&=~mask;
    }
    ui->Fuse0->setText(QString("%1h").arg(fuse[0], 2, 16, QChar('0'))); //Ustaw fusebajty w QLineEdit
    ui->Fuse1->setText(QString("%1h").arg(fuse[1], 2, 16, QChar('0')));
    ui->Fuse2->setText(QString("%1h").arg(fuse[2], 2, 16, QChar('0')));
    ui->Fuse4->setText(QString("%1h").arg(fuse[3], 2, 16, QChar('0')));
    ui->Fuse5->setText(QString("%1h").arg(fuse[4], 2, 16, QChar('0')));
    UpdateFuseByteCheckboxes();   //Update fusebyte checkboxes
    ShowAVRDudeCmdLineParams();
}

void MainDialog::LockBitChangedByUser()
{
    bool ok;
    uint8_t lock=0xff;//0;   //Unused lockbits are always unprogrammed
    Part mcu=AVR->GetPartByDescription(ui->AVRTypeCB->currentText());
    QVector<Bit> locks=mcu.GetLockBits();
    for(int i=0; i<ui->LockbitTable->rowCount(); i++)   //Scan all rows and set lockbits appropiately
    {
        QComboBox *tb=dynamic_cast<QComboBox*>(ui->LockbitTable->cellWidget(i, 0)); //Get combobox at indicated row
        if(tb)  //Je�li tb jest rzeczywi�cie potomkiem klasy QComboBox
        {
            QString str=tb->currentText();   //Pobierz wybrane ustwienie
            uint8_t mask=locks[i].GetMask().toUInt(&ok, 0); //Get bitmask of subsequent fields
            lock&=~mask; //Erase masked bits
            uint8_t offs=0;
            while((mask & (1<<offs))==0) offs++;  //Calculate needed rotation of value field
            for(int ind=0; ind<locks[i].GetValues().size(); ind++)
                if(locks[i].GetValues()[ind].GetName().compare(str)==0)
                       lock|=(locks[i].GetValues()[ind].GetValue().toUInt(&ok,0) << offs);
        }
    }
    ui->Lock_byte->setText(QString("%1h").arg(lock, 2, 16, QChar('0')));  //Ustaw nowe lockbity
    UpdateLockByteCheckboxes(lock);  //Update lockbits checkboxes
    ShowAVRDudeCmdLineParams();
}

void MainDialog::ReadLock()
{
    uint8_t lock;
    QStringList sl;
    sl<<"-c"; sl<<GetProgrammerAsAVRDudeParam();
    sl<<"-p"; sl<<GetMCUAsAVRDudeParam();
    sl<<"-P"; sl<<GetPortAsAVRDudeParam();
    GetLockBitsAVRDudeCmdParams(&sl);

    AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), QString(""), QString(""), this);
    if(AVRDude.ReadByte(&sl, lock))
    {
        //Uaktualnij lockbity
        ui->Lock_byte->setText(QString("%1h").arg(lock, 2, 16, QChar('0')));
        LockByteChanged();
    }
}

void MainDialog::WriteLock()
{
    uint8_t lock;
    QStringList sl;
    sl<<"-c"; sl<<GetProgrammerAsAVRDudeParam();
    sl<<"-p"; sl<<GetMCUAsAVRDudeParam();
    sl<<"-P"; sl<<GetPortAsAVRDudeParam();
    SetLockBitsAVRDudeCmdParams(&sl);
    //Przed ustawieniem nowych lockbit�w powinno si� odczyta� state i sprawdzi� czy nowe nie s� mniej restrykcyjne
    //Je�li s� mniej restrykcyjne to trzeba naj[ierw skasowa� chip (opcja AVRDude -e.
    //Kiedy� si� to dorobi :)
    AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), QString(""), QString(""), this);
    if(AVRDude.ReadByte(&sl, lock)==false)
    {
        QMessageBox::critical(this, tr("Lockbity"), tr("Nie uda�o si� zapisa� nowych warto�ci lockbit�w.\n Najcz�stsz� przyczyn� jest ustawienie mniej restrykcyjnego poziomu dost�pu do pami�ci. Wymaga to wcze�niejszego skasowania chipu."), QMessageBox::Ok, QMessageBox::Ok);
    } else QMessageBox::information(this, tr("Lockbity"), tr("Warto�ci lockbit�w zosta�y zapisane poprawnie."), QMessageBox::Ok, QMessageBox::Ok);
}

void MainDialog::VerifyLock()
{
    uint8_t lock;
    QStringList sl;
    sl<<"-c"; sl<<GetProgrammerAsAVRDudeParam();
    sl<<"-p"; sl<<GetMCUAsAVRDudeParam();
    sl<<"-P"; sl<<GetPortAsAVRDudeParam();
    GetLockBitsAVRDudeCmdParams(&sl);

    AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), QString(""), QString(""), this);
    if(AVRDude.ReadByte(&sl, lock))
    {
        bool ok;
        QString str=ui->Lock_byte->text().left(2);
        if(str.toUInt(&ok, 16)==lock) QMessageBox::information(this, tr("Weryfikacja lockbit�w"), tr("Ok. Zaprogramowane lockbity odpowiadaj� ustawionym."), QMessageBox::Ok, QMessageBox::Ok);
        else QMessageBox::critical(this, tr("Weryfikacja lockbit�w"), tr("Weryfikacja lockbit�w przebieg�a niepomy�lnie!\n ��dane lockbity to:%1h, a odczytane: %2h.").arg(str).arg(lock, 2, 16, QChar(' ')), QMessageBox::Ok, QMessageBox::Ok);
    }
}

void MainDialog::LockBitChBoxChg()
{
    uint8_t byte=0;
    if(ui->lock_b0->checkState()==Qt::Checked) byte|=1;   //Konwersja postaci bitowej na dec
    if(ui->lock_b1->checkState()==Qt::Checked) byte|=2;
    if(ui->lock_b2->checkState()==Qt::Checked) byte|=4;
    if(ui->lock_b3->checkState()==Qt::Checked) byte|=8;
    if(ui->lock_b4->checkState()==Qt::Checked) byte|=16;
    if(ui->lock_b5->checkState()==Qt::Checked) byte|=32;
    if(ui->lock_b6->checkState()==Qt::Checked) byte|=64;
    if(ui->lock_b7->checkState()==Qt::Checked) byte|=128;

    QString txt=QString("%1h").arg(byte, 2, 16, QChar('0'));
    ui->Lock_byte->setText(txt); //**** UWAGA!! Tekst musi by� zgodny z formatem ustawionym w Designerze, inaczej nic nie zostanie wstawione ****
    UpdateLockBitTable(byte);
    ShowAVRDudeCmdLineParams();
}

void MainDialog::LockByteChanged()
{
    bool ok;
    uint8_t val=ui->Lock_byte->text().left(2).toInt(&ok,16);
    if(ok==false) val=0xff;
    UpdateLockByteCheckboxes(val);
    UpdateLockBitTable(val);
    ShowAVRDudeCmdLineParams();
}

void MainDialog::UpdateLockByteCheckboxes(uint8_t val)
{
    ui->lock_b0->blockSignals(true); ui->lock_b1->blockSignals(true); ui->lock_b2->blockSignals(true); ui->lock_b3->blockSignals(true);
    ui->lock_b4->blockSignals(true); ui->lock_b5->blockSignals(true); ui->lock_b6->blockSignals(true); ui->lock_b7->blockSignals(true);

    if(val & 1) ui->lock_b0->setChecked(Qt::Checked); else ui->lock_b0->setChecked(Qt::Unchecked);
    if(val & 2) ui->lock_b1->setChecked(Qt::Checked); else ui->lock_b1->setChecked(Qt::Unchecked);
    if(val & 4) ui->lock_b2->setChecked(Qt::Checked); else ui->lock_b2->setChecked(Qt::Unchecked);
    if(val & 8) ui->lock_b3->setChecked(Qt::Checked); else ui->lock_b3->setChecked(Qt::Unchecked);
    if(val & 16) ui->lock_b4->setChecked(Qt::Checked); else ui->lock_b4->setChecked(Qt::Unchecked);
    if(val & 32) ui->lock_b5->setChecked(Qt::Checked); else ui->lock_b5->setChecked(Qt::Unchecked);
    if(val & 64) ui->lock_b6->setChecked(Qt::Checked); else ui->lock_b6->setChecked(Qt::Unchecked);
    if(val & 128) ui->lock_b7->setChecked(Qt::Checked); else ui->lock_b7->setChecked(Qt::Unchecked);
    ui->lock_b0->blockSignals(false); ui->lock_b1->blockSignals(false); ui->lock_b2->blockSignals(false); ui->lock_b3->blockSignals(false);
    ui->lock_b4->blockSignals(false); ui->lock_b5->blockSignals(false); ui->lock_b6->blockSignals(false); ui->lock_b7->blockSignals(false);
}

void MainDialog::BlockSignalsFromTable(QTableWidget *tbl, bool en)
{
    for (int i=tbl->rowCount()-1; i >= 0; --i) //Block all signals from table
    {
        QWidget *tb=dynamic_cast<QWidget*>(tbl->cellWidget(i, 0));
        if(tb) tb->blockSignals(en);
    }
}

void MainDialog::UpdateBitsTable(QTableWidget *tbl, QVector<Bit> &bits, uint8_t *value)
{
    bool ok;
    BlockSignalsFromTable(ui->FusebitTable, true);
    //Here we can safe modify table widgets, without emitting unnecessary signals
    for(int i=0; i<tbl->rowCount(); i++)   //Scan all rows and set lockbits appropiately
    {
        uint8_t mask=bits[i].GetMask().toUInt(&ok, 0);      //Get bitmask of subsequent fields
        uint8_t foffset=bits[i].GetOffset().toUInt(&ok, 0); //Get fusebyte offset
        QWidget *tb=tbl->cellWidget(i, 0);      //Get wiget at indicated row
        if(dynamic_cast<QComboBox*>(tb))  //Uzupe�nij comboboxy
        {
            uint8_t offs=0;
            while((mask & (1<<offs))==0) offs++;  //Calculate needed rotation of value field
            uint8_t ff=(value[foffset] & mask) >> offs;
            for(int ind=0; ind<bits[i].GetValues().size(); ind++)
                if(bits[i].GetValues()[ind].GetValue().toUInt(&ok, 0)==ff)
                      ((QComboBox*)tb)->setCurrentIndex(ind);  //Set combobox according to lockbit value
        } else
            if(dynamic_cast<QCheckBox*>(tb))   //Uzupe�nij checkboxy
            { if(value[foffset] & mask) ((QCheckBox*)tb)->setChecked(Qt::Unchecked); else ((QCheckBox*)tb)->setChecked(Qt::Checked); };
    }
    BlockSignalsFromTable(ui->FusebitTable, false);
}

void MainDialog::UpdateLockBitTable(uint8_t val)
{
    Part mcu=AVR->GetPartByDescription(ui->AVRTypeCB->currentText());
    QVector<Bit> locks=mcu.GetLockBits();
    UpdateBitsTable(ui->LockbitTable, locks, &val);
}

void MainDialog::GetFuseBytesFromEditLines(uint8_t fuses[5])
{
    bool ok;
    fuses[0]=ui->Fuse0->text().left(2).toInt(&ok,16);
    fuses[1]=ui->Fuse1->text().left(2).toInt(&ok,16);
    fuses[2]=ui->Fuse2->text().left(2).toInt(&ok,16);
    fuses[3]=ui->Fuse4->text().left(2).toInt(&ok,16);
    fuses[4]=ui->Fuse5->text().left(2).toInt(&ok,16);
}

void MainDialog::UpdateFusekBitTable()
{
    if(AVR)    //Je�li nie uda�o si� otworzy� pliku konfiguracyjnego to nie ma co szuka� informacji o MCU
    {
        uint8_t fuse[5];
        GetFuseBytesFromEditLines(fuse);   //Pobierz fusebajty z linii edycji
        Part mcu=AVR->GetPartByDescription(ui->AVRTypeCB->currentText());
        QVector<Bit> fuses=mcu.GetFuseBits();
        UpdateBitsTable(ui->FusebitTable, fuses, fuse);
    }
}

void MainDialog::GetLockBitsAVRDudeCmdParams(QStringList *params)
{
    *params << "-Ulock:r:-:h"; //Odczytaj lockbity, wy�wielt warto�� na ekranie w postaci hex
}

void MainDialog::SetLockBitsAVRDudeCmdParams(QStringList *params)
{
    bool ok;
    uint8_t lock=ui->Lock_byte->text().left(2).toInt(&ok,16);  //Read lockbits value
    if(ok==false) lock=0xff;
    *params << QString("-Ulock:w:0x%1:m").arg(lock, 2, 16, QChar('0'));  //Generate aVRDude command
}

void MainDialog::GetFuseBitsAVRDudeCmdParams(QStringList *params)
{
//params = :);
}

void MainDialog::SetFuseBitsAVRDudeCmdParams(QStringList *params)
{
//params = :);
}

uint8_t MainDialog::HowManyFuseBytes()
{
    uint8_t fuseoffset=0;
    Part mcu=AVR->GetPartByDescription(ui->AVRTypeCB->currentText());
    QVector<Bit> fuses=mcu.GetFuseBits();
    for(int i=0; i<fuses.size(); i++)   //Scan all fusebytes
    {
        uint8_t offs=fuses[i].GetOffset().mid(2,2).toUInt(); //Convert offset from string to uint
        if(offs>fuseoffset) fuseoffset=offs;  //Check how many fusebytes we have
    }
    return fuseoffset;
}

void MainDialog::FuseByteChanged()
{
    int FuseIndex=0;
    QList<QLineEdit*> le = findChildren<QLineEdit*>(QRegExp("Fuse*"));  //Znajd� wszystkie obiekty QLineEdit zawieraj�ce fusebajty
    while((FuseIndex<le.size()) && (le.at(FuseIndex)->hasFocus()==false)) FuseIndex++;
    if(FuseIndex==le.size()) FuseIndex=4;  //In case no QLineEdit is focused. By default the first fuse QLineEdit is the last on QList record.
    if(FuseIndex<le.size())
    {
        QList<QLineEdit*> tle = findChildren<QLineEdit*>(LastSelFuseByte);
        tle.at(0)->setPalette(editpal);              //Odtw�rz zapisany kolor dla deaktywowanego okienka

        QPalette pal=le.at(FuseIndex)->palette();    //Pobierz kolor widgetu
        editpal=pal;                                 //Zapisz oryginalny kolor
        pal.setColor(QPalette::Text, pal.color(QPalette::Highlight));
        le.at(FuseIndex)->setPalette(pal);

        LastSelFuseByte=le.at(FuseIndex)->objectName();  //Nazwa obiektu
        UpdateFuseByteCheckboxes();
    }
    UpdateFusekBitTable();
    ShowAVRDudeCmdLineParams();
}

void MainDialog::UpdateFuseByteCheckboxes()
{
    QList<QLineEdit*> le = findChildren<QLineEdit*>(QRegExp(LastSelFuseByte));  //Znajd� wybrany obiekt QLineEdit b�d�cy ostatnio w u�yciu
    if(le.size())
    {
        bool ok;
        int val=le.at(0)->text().left(2).toInt(&ok,16);  //Get fuse value
        if(ok==false) val=0xff;
        ui->Fuse_b0->blockSignals(true); ui->Fuse_b1->blockSignals(true); ui->Fuse_b2->blockSignals(true); ui->Fuse_b3->blockSignals(true);
        ui->Fuse_b4->blockSignals(true); ui->Fuse_b5->blockSignals(true); ui->Fuse_b6->blockSignals(true); ui->Fuse_b7->blockSignals(true);
        if(val & 1) ui->Fuse_b0->setChecked(Qt::Checked); else ui->Fuse_b0->setChecked(Qt::Unchecked);
        if(val & 2) ui->Fuse_b1->setChecked(Qt::Checked); else ui->Fuse_b1->setChecked(Qt::Unchecked);
        if(val & 4) ui->Fuse_b2->setChecked(Qt::Checked); else ui->Fuse_b2->setChecked(Qt::Unchecked);
        if(val & 8) ui->Fuse_b3->setChecked(Qt::Checked); else ui->Fuse_b3->setChecked(Qt::Unchecked);
        if(val & 16) ui->Fuse_b4->setChecked(Qt::Checked); else ui->Fuse_b4->setChecked(Qt::Unchecked);
        if(val & 32) ui->Fuse_b5->setChecked(Qt::Checked); else ui->Fuse_b5->setChecked(Qt::Unchecked);
        if(val & 64) ui->Fuse_b6->setChecked(Qt::Checked); else ui->Fuse_b6->setChecked(Qt::Unchecked);
        if(val & 128) ui->Fuse_b7->setChecked(Qt::Checked); else ui->Fuse_b7->setChecked(Qt::Unchecked);
        ui->Fuse_b0->blockSignals(false); ui->Fuse_b1->blockSignals(false); ui->Fuse_b2->blockSignals(false); ui->Fuse_b3->blockSignals(false);
        ui->Fuse_b4->blockSignals(false); ui->Fuse_b5->blockSignals(false); ui->Fuse_b6->blockSignals(false); ui->Fuse_b7->blockSignals(false);
    }
}

void MainDialog::BitChBoxChg(int)
{
    int byte=0;
    if(ui->Fuse_b0->checkState()==Qt::Checked) byte|=1;   //Konwersja postaci bitowej na dec
    if(ui->Fuse_b1->checkState()==Qt::Checked) byte|=2;
    if(ui->Fuse_b2->checkState()==Qt::Checked) byte|=4;
    if(ui->Fuse_b3->checkState()==Qt::Checked) byte|=8;
    if(ui->Fuse_b4->checkState()==Qt::Checked) byte|=16;
    if(ui->Fuse_b5->checkState()==Qt::Checked) byte|=32;
    if(ui->Fuse_b6->checkState()==Qt::Checked) byte|=64;
    if(ui->Fuse_b7->checkState()==Qt::Checked) byte|=128;

    QString txt=QString("%1h").arg(byte, 2, 16, QChar('0'));
    QList<QLineEdit*> le = findChildren<QLineEdit*>(LastSelFuseByte);  //Znajd� ostatnio wybrany obiekt QLineEdit zawieraj�cy fusebajty
    if(le.size()) le.at(0)->setText(txt); //**** UWAGA!! Tekst musi by� zgodny z formatem ustawionym w Designerze, inaczej nic nie zostanie wstawione ****
    UpdateFusekBitTable();
    ShowAVRDudeCmdLineParams();
}

void MainDialog::ReadFuse()
{

}

void MainDialog::WriteFuse()
{

}

void MainDialog::VerifyFuse()
{

}

void MainDialog::DefaultFuse()
{

}

void MainDialog::EnableFuseBytes()
{
    uint8_t fuseoffset=HowManyFuseBytes();
    switch(fuseoffset)
    {
    case 0:     ui->Fuse0->setEnabled(true); ui->Fuse1->setEnabled(false); ui->Fuse2->setEnabled(false); ui->Fuse4->setEnabled(false); ui->Fuse5->setEnabled(false); break;
    case 1:     ui->Fuse0->setEnabled(true); ui->Fuse1->setEnabled(true); ui->Fuse2->setEnabled(false); ui->Fuse4->setEnabled(false); ui->Fuse5->setEnabled(false); break;
    case 2:     ui->Fuse0->setEnabled(true); ui->Fuse1->setEnabled(true); ui->Fuse2->setEnabled(true); ui->Fuse4->setEnabled(false); ui->Fuse5->setEnabled(false); break;
    case 3:     ui->Fuse0->setEnabled(true); ui->Fuse1->setEnabled(true); ui->Fuse2->setEnabled(true); ui->Fuse4->setEnabled(true); ui->Fuse5->setEnabled(false); break;
    case 4:     ui->Fuse0->setEnabled(true); ui->Fuse1->setEnabled(true); ui->Fuse2->setEnabled(true); ui->Fuse4->setEnabled(true); ui->Fuse5->setEnabled(true); break;
    };
}

void MainDialog::ProgrammBtn()
{
    SimpPgmDialog dlg(this);
    if(dlg.exec()==QMessageBox::Ok)
    {
        ui->SimpProgrammBtn->setText("Przerwij");
        AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), ui->FLASHFile->text(), ui->EEPROMFile->text(), this);
        int res=dlg.GetResult();   //Co wybrał użytkownik
        AVRDude.ProgramMemories(res, ui->SimpProgrammProgressBar); //Rozpocznij programowanie
        ui->SimpProgrammBtn->setText("Programuj");
    }
}

void MainDialog::EnablePgmButtons()
{
    bool en=true;
    bool FLASHen=true;
    bool EEPROMen=true;

    if(ui->AVRTypeCB->currentText().compare("<Auto>")==0) en=false;  //Nie możemy odblokować jeśli nie wybrano typu MCU
    if(ui->FLASHFile->text().size()==0) FLASHen=false;               //Nie można programować jeśli nie ma pliku z FLASHEM
    if(ui->EEPROMFile->text().size()==0) EEPROMen=false;             //Lub pliku z EEPROM

    ui->SimpProgrammBtn->setEnabled(en);  //Odblokuj przycisk programowania na zakładce uproszczonego programowania

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

    QSettings appsettings;                               //Odczytaj �cie�k� do AVRDude (je�li jest)
     appsettings.beginGroup("MainWindow");
     path=appsettings.value("AVRDudePath").toString();
     emit SetAVRDudePath(path);                          //Uzupe�nij pole edycji �cie�ki do AVRDude

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setDirectory(path);
    QStringList fileNames;
    if (dialog.exec())    //Wy�wietl dialog wyboru �cie�ki
     {
        fileNames = dialog.selectedFiles();
        if(fileNames.isEmpty()==false)
        {
            path=fileNames.at(0);        //Odczytaj �cie�k�
            appsettings.setValue("AVRDudePath", path);
            emit SetAVRDudePath(path);
        }
     }

    appsettings.endGroup();  //Zapisz zmiany
    ReLoadAVRDudeConf();                        //Prze�aduj konfiguracj� AVRDude
}

void MainDialog::SavePathToAVRDude(QString path)
{
    QSettings appsettings;                               //Zapisz �cie�k� do AVRDude
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("AVRDudePath", path);
     appsettings.endGroup();  //Zapisz zmiany
}

void MainDialog::SavePathToAVRBinutils(QString path)
{
    QSettings appsettings;                               //Zapisz �cie�k� do binutils
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("AVRBinutilsPath", path);
     appsettings.endGroup();  //Zapisz zmiany
}

void MainDialog::AVRBinutilsSetPath()
{
    QString path;

    QSettings appsettings;                               //Odczytaj �ci�k� do binutils (je�li jest)
     appsettings.beginGroup("MainWindow");
     path=appsettings.value("AVRBinutilsPath").toString();
     emit SetBinutilsPath(path);                          //Uzupełnij pole edycji ścieżki do AVRDude

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setDirectory(path);
    QStringList fileNames;
    if (dialog.exec())    //Wy�wietl dialog wyboru �cie�ki
     {
        fileNames = dialog.selectedFiles();
        if(fileNames.isEmpty()==false)
        {
            path=fileNames.at(0);        //Odczytaj �cie�k�
            appsettings.setValue("AVRBinutilsPath", path);
            emit SetBinutilsPath(path);
        }
     }
    appsettings.endGroup();  //Zapisz zmiany
}

void MainDialog::OpenFLASHFileDlg()
{
    QString path;

    QSettings appsettings;                               //Odczytaj ścieżkę do pliku FLASH
     appsettings.beginGroup("MainWindow");
     path=appsettings.value("FLASHFilePath").toString();
     appsettings.endGroup();  //Zapisz zmiany

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(GetSupportedFileExtensions());
    dialog.setDirectory(path);
    QStringList fileNames;
    if (dialog.exec())    //Wyświetl dialog wyboru ścieżki
     {
        fileNames = dialog.selectedFiles();
        if(!fileNames.isEmpty())
        {
            path=fileNames.at(0);        //Odczytaj ścieżkę
            emit SetFLASHFile(path);     //Uaktualnij pola w których występuje odwołanie do ścieżki pliku hex

            if((ui->AutoEEPBtn->checkState()==Qt::Checked) && (path.endsWith(".hex", Qt::CaseInsensitive)))  //Podano plik o rozszerzeniu .hex, szukamy więc czy jest .eep
            {                                                                                                //o ile wybrano opcję automatyczneg ładowania EEPROM
                path.truncate(path.size()-4);  //Wycinamy ".hex"
                path.append(".eep");
                if(QFile(path).exists()) emit SetEEPROMFile(path);  //Uaktualnij pola w których znajduje się ścieżka do pliku eep
            }
            AVRDudeCmdLineParams();      //Uaktualnij listę polecenia AVRDude, aby zawierała nazwy nowowybranych plików
        }
     }
}

void MainDialog::OpenEEPROMFileDlg()
{
    QString path;

    QSettings appsettings;                               //Odczytaj ścieżkę do AVRDude (jeśli jest)
     appsettings.beginGroup("MainWindow");
     path=appsettings.value("EEPROMFilePath").toString();
     appsettings.endGroup();

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("IntelHex (*.eep)"));
    dialog.setDirectory(path);
    QStringList fileNames;
    if (dialog.exec())    //Wyświetl dialog wyboru ścieżki
     {
        fileNames = dialog.selectedFiles();
        if(!fileNames.isEmpty())
        {
            path=fileNames.at(0);        //Odczytaj ścieżkę
            emit SetEEPROMFile(path);                      //Uaktualnij pola w których występuje odwołanie do ścieżki pliku hex
        }
     }
}

void MainDialog::FillProgrammerCB()
{
    ui->ProgrammerCB->clear();   //Skasuj poprzednie pozycje
    if(AVR)
    {
        QVector<Programmer> pgm=AVR->GetProgrammers();  //Lista programatorów obsługiwanych przez AVRDude

        for (int i = 0; i < pgm.size(); ++i)
        {
            ui->ProgrammerCB->addItem(pgm[i].GetID());         //Wypełnij combo z tympami obsługiwanych programatorów
        }
    }
}

void MainDialog::FillPortCB()
{
    ui->PortCB->clear();
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();  //Pobierz nazwy dostępnych portów szeregowych
    for (int i = 0; i < ports.size(); i++)
    {
        ui->PortCB->addItem(ports.at(i).portName);
    }

    ui->PortCB->addItem("usb");
}

void MainDialog::FillMCUType()
{
    ui->AVRTypeCB->clear();   //Skasuj poprzednie pozycje
    if(AVR)
    {
        QVector<Part> pgm=AVR->GetParts();  //Lista programatorów obsługiwanych przez AVRDude
        for (int i = 0; i < pgm.size(); ++i) ui->AVRTypeCB->addItem(pgm[i].GetDescription());         //Wypełnij combo z tympami obsługiwanych procesorów
    }
}

void MainDialog::TestConnection()
{
    AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), QString(""), QString(""), this);
    QString MCUSig=AVRDude.LookForMCU();
    AVRDudeExecutor::Errors err=AVRDude.GetExecErr();   //Czy wszystko się skończyło ok?
    if(err==AVRDudeExecutor::Err_Ok)
    {
        ui->AVRSignatureValueLBL->setText(MCUSig);   //Wyświetl znalezioną sygnaturę
        QString MCU=AVR->GetPartBySignature(MCUSig).GetDescription();
        ui->AVRTypeCB->setCurrentIndex(ui->AVRTypeCB->findText(MCU));  //Znajdź MCU na podstawie sygnatury
        MCUChanged(MCU);
        ui->ConnectionOk->setText(tr("Połączenie ok"));
    } else
    {
        ui->ConnectionOk->setText(tr("Brak połączenia"));
        QMessageBox::information(this, tr("AVRDude - błąd"),
                                 tr("Nie znaleziono sygnatury urządzenia !\nByć może nie jest podłączone lub wystąpił problem z połączeniem."), QMessageBox::Ok, QMessageBox::Ok);
    }
}

void MainDialog::HideAdvancedTabs(bool hide)
{
    for(int i = 1; i < 4; i++)
        ui->Tabs->setTabEnabled(i, !hide);
}

void MainDialog::DisableTabs(bool dis)
{
    for(int i = 0; i < 4; i++)
        ui->Tabs->setTabEnabled(i, !dis);
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
     ui->AVRDudeCMDLineGroupBox->setVisible(state==Qt::Checked);  //Ukryj lub pokaż okienko z linią polecenia AVRDude
}

void MainDialog::SavePathToEEPROMFile(QString file)
{
    QSettings appsettings;                               //Zapisz ścieżkę do pliku eep
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("EEPROMFilePath", file);
     appsettings.endGroup();  //Zapisz zmiany
}

void MainDialog::SavePathToFLASHFile(QString file)
{
    QSettings appsettings;                               //Zapisz ścieżkę do pliku eep
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("FLASHFilePath", file);
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
    QString sig=AVR->GetPartByDescription(text).GetSignature();  //Znajd� opis wybranego MCU
    ui->AVRSignatureValueLBL->setText(sig);
    ui->StatusLBL->setText(tr("Procesor: ").append(text));   //Uaktualnij lini� stanu wskazuj�c� na typ procesora

    QSettings appsettings;                               //Zapisz wybrany mikrokontroler
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("MCU", text);
     appsettings.endGroup();  //Zapisz zmiany

     UpdateFuseBitsWidget();     //Uaktualnij zak�adk� fusebit�w
     UpdateLockBitsWidget();     //Uaktualnij zak�adk� lockbit�w
}

void MainDialog::EraseFLASHChBox(int state)
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("EraseFLASH", state);
     appsettings.endGroup();  //Zapisz zmiany

     AVRDudeCmdLineParams();    //Odśwież okienko stanu
}

void MainDialog::ReadFLASH()
{
    QString Filename=GetFLASHFilePath(); //Nazwa pliku FLASH
    if(QFile(Filename).exists())
    {
        int res=QMessageBox::information(this, tr("Plik"), tr("Plik %1 istnieje. Nadpisać go?").arg(Filename), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        switch(res)
        {
        case QMessageBox::Yes: break;
        case QMessageBox::No:  return;   //Nic nie sta�o
        };

        QStringList sl;
        ReturnBasicAVRDudeParams(sl);
        sl<<QString("-Uflash:r:%1:i").arg(QFileInfo(GetFLASHFilePath()).fileName());
        AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), GetFLASHFilePath(), QString(""), this);
        AVRDude.MemoryOp(sl, "Czytam pami�� FLASH"); //Odczytaj pami��
    }
}

void MainDialog::WriteFLASH()
{
    QString Filename=GetFLASHFilePath(); //Nazwa pliku FLASH
    if(QFile(Filename).exists())
    {
        QStringList sl;
        ReturnBasicAVRDudeParams(sl);
        sl<<GetPerformEraseChipAsAVRDudeParam();   //Czy kasowa� chip?
        sl<<QString("-Uflash:w:%1:i").arg(QFileInfo(GetFLASHFilePath()).fileName());
        AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), GetFLASHFilePath(), QString(""), this);
        AVRDude.MemoryOp(sl, "Zapisuj� pami�� FLASH"); //Zapisz pami��
    } else QMessageBox::information(this, tr("Plik"), tr("Plik %1 nie istnieje.").arg(Filename), QMessageBox::Ok, QMessageBox::Ok);
}

void MainDialog::VerifyFLASH()
{
    QString Filename=GetFLASHFilePath(); //Nazwa pliku FLASH
    if(QFile(Filename).exists())
    {
        QStringList sl;
        ReturnBasicAVRDudeParams(sl);
        sl<<QString("-Uflash:v:%1:i").arg(QFileInfo(GetFLASHFilePath()).fileName());
        AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), GetFLASHFilePath(), QString(""), this);
        AVRDude.MemoryOp(sl, "Weryfikuj� pami�� FLASH"); //Weryfikuj pami��
    } else QMessageBox::information(this, tr("Plik"), tr("Plik %1 nie istnieje.").arg(Filename), QMessageBox::Ok, QMessageBox::Ok);
}

void MainDialog::ReadEEPROM()
{
    QString Filename=GetEEPROMFilePath(); //Nazwa pliku EEPROM
    if(QFile(Filename).exists())
    {
        int res=QMessageBox::information(this, tr("Plik"), tr("Plik %1 istnieje. Nadpisać go?").arg(Filename), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        switch(res)
        {
        case QMessageBox::Yes: break;
        case QMessageBox::No:  return;   //Nic nie sta�o
        };

        QStringList sl;
        ReturnBasicAVRDudeParams(sl);
        sl<<QString("-Ueeprom:r:%1:i").arg(QFileInfo(GetEEPROMFilePath()).fileName());
        AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), GetFLASHFilePath(), GetEEPROMFilePath(), this);
        AVRDude.MemoryOp(sl, "Czytam pami�� EEPROM"); //Odczytaj pami��
    }
}

void MainDialog::WriteEEPROM()
{
    QString Filename=GetEEPROMFilePath(); //Nazwa pliku EEPROM
    if(QFile(Filename).exists())
    {
        QStringList sl;
        ReturnBasicAVRDudeParams(sl);
        sl<<GetPerformEraseChipAsAVRDudeParam();   //Czy kasowa� chip?
        sl<<QString("-Ueeprom:w:%1:i").arg(QFileInfo(GetEEPROMFilePath()).fileName());
        AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), GetFLASHFilePath(), GetEEPROMFilePath(), this);
        AVRDude.MemoryOp(sl, "Zapisuj� pami�� EEPROM"); //Zapisz pami��
    } else QMessageBox::information(this, tr("Plik"), tr("Plik %1 nie istnieje.").arg(Filename), QMessageBox::Ok, QMessageBox::Ok);
}

void MainDialog::VerifyEEPROM()
{
    QString Filename=GetEEPROMFilePath(); //Nazwa pliku EEPROM
    if(QFile(Filename).exists())
    {
        QStringList sl;
        ReturnBasicAVRDudeParams(sl);
        sl<<QString("-Ueeprom:v:%1:i").arg(QFileInfo(GetEEPROMFilePath()).fileName());
        AVRDudeExecutor AVRDude(GetProgrammerAsAVRDudeParam(), GetPortAsAVRDudeParam(), GetMCUAsAVRDudeParam(), GetEEPROMFilePath(), GetEEPROMFilePath(), this);
        AVRDude.MemoryOp(sl, "Weryfikuj� pami�� EEPROM"); //Weryfikuj pami��
    } else QMessageBox::information(this, tr("Plik"), tr("Plik %1 nie istnieje.").arg(Filename), QMessageBox::Ok, QMessageBox::Ok);
}

void MainDialog::DisableEEPROMIfElf(QString file)
{
    QString ext=QFileInfo(file).suffix();
    if(ext.compare("elf")==0)   //Je�li plik elf to blokujemy wszystko co jest zwi�zane z EEPROM
    {
        ui->EEPROMBox->setEnabled(false);
    } else ui->EEPROMBox->setEnabled(true);
}

void MainDialog::VerifyFLASHChBox(int state)
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");

     appsettings.setValue("VerifyFLASH", state);
     appsettings.endGroup();  //Zapisz zmiany

     AVRDudeCmdLineParams();    //Odśwież okienko stanu
}

QString MainDialog::GetMCUAsAVRDudeParam()
{
    QString str;

    if(AVR) str=AVR->GetPartByDescription(ui->AVRTypeCB->currentText()).GetID();
    return str;
}

QString MainDialog::GetProgrammerAsAVRDudeParam()
{
    QString str=ui->ProgrammerCB->currentText();
    return str;
}

QString MainDialog::GetPortAsAVRDudeParam()
{
    QString str=ui->PortCB->currentText().toLower(); //Don't know why, but AVRDude works better with port specified in lowercase.
    return str;
}

QString MainDialog::GetPerformEraseChipAsAVRDudeParam()
{
    QString str="";
    if(ui->EraseBox->checkState()!=Qt::Checked) str.append(" -D ");  //Nie wykonuj chip erase przed programowaniem
    return str;
}

void MainDialog::ShowAVRDudeCmdLineParams()
{
    AVRDudeCmdLineParams();
}

void MainDialog::ReturnBasicAVRDudeParams(QStringList &sl)
{
    sl << "-p"; sl << GetMCUAsAVRDudeParam();
    sl << "-c"; sl << GetProgrammerAsAVRDudeParam();
    sl << "-P"; sl << GetPortAsAVRDudeParam();
}

void MainDialog::AVRDudeCmdLineParams()
{
    QString str="avrdude ";
    QStringList sl;
    ReturnBasicAVRDudeParams(sl);
    sl << GetPerformEraseChipAsAVRDudeParam();
    sl << *AVRDudeExecutor::GetAVRDudeCmdMemProgramm(GetFLASHFilePath(), GetEEPROMFilePath(), ui->VerifyBox->checkState());

    SetFuseBitsAVRDudeCmdParams(&sl);  //Add fusebits command
    SetLockBitsAVRDudeCmdParams(&sl);  //Add lockbits command

    str.append(sl.join(QString(" ")));
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

QString MainDialog::GetBinutilsPath()
{
    QString path;
     QSettings appsettings;
     appsettings.beginGroup("MainWindow");
     path=appsettings.value("AVRBinutilsPath", "").toString();   //Odczytaj �cie�k� do binutils
     appsettings.endGroup();

     if((QFileInfo(path, AVROBJCOPY).exists()==false) || (QFileInfo(path, AVROBJDUMP).exists()==false)) path="";  //Zwr�� pust� �cie�k� je�li kt�ry� z wymaganych plik�w nie istnieje

    return path;
}

QString MainDialog::GetSupportedFileExtensions()
{
    if(GetBinutilsPath().size()) return tr("IntelHex (*.hex);;Executable and Linkable Format (*.elf)"); //Sprawd� czy s� dost�pne narz�dzia binutils
    return tr("IntelHex (*.hex)");
}

MainDialog::~MainDialog()
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");     //Save mainwindow size and position
     appsettings.setValue("size", size());
     appsettings.setValue("pos", pos());
     appsettings.endGroup();

    delete ui;
    delete AVR;
}
