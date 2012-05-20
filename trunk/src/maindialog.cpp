#include <QSettings>
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
}

MainDialog::~MainDialog()
{
    QSettings appsettings;
     appsettings.beginGroup("MainWindow");     //Save mainwindow size and position
     appsettings.setValue("size", size());
     appsettings.setValue("pos", pos());
     appsettings.endGroup();

    delete ui;
}
