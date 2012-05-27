#include <QSettings>
#include <QMessageBox>
#include "simppgmdlg.h"

SimpPgmDialog::SimpPgmDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::SimpPgmDialog)
{
    ui->setupUi(this);
    setFixedHeight(174);

    QSettings appsettings;
     appsettings.beginGroup("SimpPgmDialog");
      ui->EEPROMCBox->setCheckState((Qt::CheckState)appsettings.value("EEPROM", Qt::Unchecked).toInt());
      ui->FLASHCBox->setCheckState((Qt::CheckState)appsettings.value("FLASH", Qt::Unchecked).toInt());
      ui->FuseCBox->setCheckState((Qt::CheckState)appsettings.value("FuseBits", Qt::Unchecked).toInt());
      ui->LockCBox->setCheckState((Qt::CheckState)appsettings.value("LockBits", Qt::Unchecked).toInt());
     appsettings.endGroup();

     setResult(QMessageBox::Cancel);
}

int SimpPgmDialog::GetResult()
{
    int res=0;
    if(ui->EEPROMCBox->checkState()) res|=R_EEPROM;
    if(ui->FLASHCBox->checkState()) res|=R_FLASH;
    if(ui->FuseCBox->checkState()) res|=R_FUSE;
    if(ui->LockCBox->checkState()) res|=R_LOCK;
}

void SimpPgmDialog::Programm()
{
    QSettings appsettings;
     appsettings.beginGroup("SimpPgmDialog");     //Save checkboxes
     appsettings.setValue("EEPROM", ui->EEPROMCBox->checkState());
     appsettings.setValue("FLASH", ui->FLASHCBox->checkState());
     appsettings.setValue("FuseBits", ui->FuseCBox->checkState());
     appsettings.setValue("LockBits", ui->LockCBox->checkState());
    appsettings.endGroup();

    //setResult(QMessageBox::Ok);
    //close();
    done(QMessageBox::Ok);
}

 SimpPgmDialog::~SimpPgmDialog()
{
     delete ui;
}
