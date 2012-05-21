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
    void HideAdvancedTabs(bool);     //Ukryj zak�adki z opcjami zaawansowanymi

    ~MainDialog();

signals:
 void SetAVRDudePath(QString);        //Ustaw �cie�k� w zak�adce SetUp do AVRDude
 void SetFLASHFile(QString);          //Ustaw �cie�k� do pliku FLASH w zak�adce "Uproszczone programowanie"
 void SetSimplifierViewChBox(bool);   //Ustaw checkbox widoku uproszczonego w zak�adce Setup
 void SetAVRDudeWindowOnError(bool);  //Ustaw checkbox wy�wietlania okna AVRD Dude w przypadku b��du
 void SetAVRDudeCMDLine(bool);        //Ustaw checkbox wy�wietlania linii polecenia AVR Dude

private slots:
    void AVRDudeSetPath();            //Ustaw �cie�k� do programu AVRDude
    void OpenFLASHFileDlg();          //Ustaw �cie�k� do pliku z FLASH
    //Z okienka SetUp
    void SetupShowSimplifiedView(int);     //Checkbox pokazuj�cy tylko widok uproszczony
    void SetupShowAVDDudeWindow(int);      //Checkbox - poka� okno AVRDude tylko je�li wyst�pi� b��d
    void SetupShowAVRDudeCmd(int);         //Poka� wygenerowane polecenie AVRDude

private:
    Ui::MainDialog *ui;
};

#endif // MAINDIALOG_H