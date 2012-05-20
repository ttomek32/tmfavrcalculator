#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

signals:
 void SetAVRDudePath(QString);        //Ustaw �cie�k� w zak�adce SetUp do AVRDude
 void SetFLASHFile(QString);          //Ustaw �cie�k� do pliku FLASH w zak�adce "Uproszczone programowanie"

private slots:
    void AVRDudeSetPath();            //Ustaw �cie�k� do programu AVRDude
    void OpenFLASHFileDlg();          //Ustaw �cie�k� do pliku z FLASH

private:
    Ui::MainDialog *ui;
};

#endif // MAINDIALOG_H
