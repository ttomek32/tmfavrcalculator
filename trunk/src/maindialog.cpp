#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

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
     path=appsettings.value("AVRDudePath").toString();   //Odczytaj �cie�k� do AVRDude (je�li jest)
     appsettings.endGroup();
    emit SetAVRDudePath(path);                          //Uzupe�nij pole edycji �cie�ki do AVRDude

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
}

void MainDialog::OpenFLASHFileDlg()
{
    QString path;

    QSettings appsettings;                               //Odczytaj �cie�k� do AVRDude (je�li jest)
     appsettings.beginGroup("MainWindow");
     path=appsettings.value("FLASHFilePath").toString();
     emit SetFLASHFile(path);                          //Uzupe�nij pole edycji �cie�ki do AVRDude

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("IntelHex (*.hex)"));
    dialog.setDirectory(path);
    QStringList fileNames;
    if (dialog.exec())    //Wy�wietl dialog wyboru �cie�ki
     {
        fileNames = dialog.selectedFiles();
        if(fileNames.isEmpty()==false)
        {
            path=fileNames.at(0);        //Odczytaj �cie�k�
            appsettings.setValue("FLASHFilePath", path);
            emit SetFLASHFile(path);
        }
     }

    appsettings.endGroup();  //Zapisz zmiany
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
