#include "AVRDudeErrorWindow.h"

AVRDudeErrorWindow::AVRDudeErrorWindow(QWidget *parent) :
    QDialog(parent), ui(new Ui::AVRDudeErrorWindow)
{
    ui->setupUi(this);
/*    setFixedHeight(174);

    QSettings appsettings;
     appsettings.beginGroup("SimpPgmDialog");
      ui->EEPROMCBox->setCheckState((Qt::CheckState)appsettings.value("EEPROM", Qt::Unchecked).toInt());
      ui->FLASHCBox->setCheckState((Qt::CheckState)appsettings.value("FLASH", Qt::Unchecked).toInt());
      ui->FuseCBox->setCheckState((Qt::CheckState)appsettings.value("FuseBits", Qt::Unchecked).toInt());
      ui->LockCBox->setCheckState((Qt::CheckState)appsettings.value("LockBits", Qt::Unchecked).toInt());
     appsettings.endGroup();*/

     //setResult(QDialog::Accepted);
}

int AVRDudeErrorWindow::GetResult()
{
    int res=0;
    /*if(ui->EEPROMCBox->checkState()) res|=R_EEPROM;
    if(ui->FLASHCBox->checkState()) res|=R_FLASH;
    if(ui->FuseCBox->checkState()) res|=R_FUSE;
    if(ui->LockCBox->checkState()) res|=R_LOCK;*/

    return res;
}

AVRDudeErrorWindow::~AVRDudeErrorWindow()
{
     delete ui;
}
