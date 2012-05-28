#include "AVRDudeErrorWindow.h"
#include <QSettings>

AVRDudeErrorWindow::AVRDudeErrorWindow(QWidget *parent) :
    QDialog(parent), ui(new Ui::AVRDudeErrorWindow)
{
    fin=false;
    QSettings appsettings;
     appsettings.beginGroup("AVRDudeErrorWindow");
     resize(appsettings.value("size", QSize(640, 480)).toSize());   //restore main window size and position
     move(appsettings.value("pos", QPoint(0, 0)).toPoint());
     appsettings.endGroup();

     ui->setupUi(this);

     //setResult(QDialog::Accepted);
}

void AVRDudeErrorWindow::reject()
{
    QSettings appsettings;
     appsettings.beginGroup("AVRDudeErrorWindow");     //Save mainwindow size and position
     appsettings.setValue("size", size());
     appsettings.setValue("pos", pos());
     appsettings.endGroup();

     fin=true;
     QDialog::reject();
}

AVRDudeErrorWindow::~AVRDudeErrorWindow()
{
    delete ui;
}
