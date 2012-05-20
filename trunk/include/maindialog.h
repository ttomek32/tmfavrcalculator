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
 void SetAVRDudePath(QString);        //Ustaw œcie¿kê w zak³adce SetUp do AVRDude
 void SetFLASHFile(QString);          //Ustaw œcie¿kê do pliku FLASH w zak³adce "Uproszczone programowanie"

private slots:
    void AVRDudeSetPath();            //Ustaw œcie¿kê do programu AVRDude
    void OpenFLASHFileDlg();          //Ustaw œcie¿kê do pliku z FLASH

private:
    Ui::MainDialog *ui;
};

#endif // MAINDIALOG_H
