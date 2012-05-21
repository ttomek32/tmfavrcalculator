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
    void HideAdvancedTabs(bool);     //Ukryj zak³adki z opcjami zaawansowanymi

    ~MainDialog();

signals:
 void SetAVRDudePath(QString);        //Ustaw œcie¿kê w zak³adce SetUp do AVRDude
 void SetFLASHFile(QString);          //Ustaw œcie¿kê do pliku FLASH w zak³adce "Uproszczone programowanie"
 void SetSimplifierViewChBox(bool);   //Ustaw checkbox widoku uproszczonego w zak³adce Setup
 void SetAVRDudeWindowOnError(bool);  //Ustaw checkbox wyœwietlania okna AVRD Dude w przypadku b³êdu
 void SetAVRDudeCMDLine(bool);        //Ustaw checkbox wyœwietlania linii polecenia AVR Dude

private slots:
    void AVRDudeSetPath();            //Ustaw œcie¿kê do programu AVRDude
    void OpenFLASHFileDlg();          //Ustaw œcie¿kê do pliku z FLASH
    //Z okienka SetUp
    void SetupShowSimplifiedView(int);     //Checkbox pokazuj¹cy tylko widok uproszczony
    void SetupShowAVDDudeWindow(int);      //Checkbox - poka¿ okno AVRDude tylko jeœli wyst¹pi³ b³¹d
    void SetupShowAVRDudeCmd(int);         //Poka¿ wygenerowane polecenie AVRDude

private:
    Ui::MainDialog *ui;
};

#endif // MAINDIALOG_H
