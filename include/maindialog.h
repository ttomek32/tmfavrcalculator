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
 void SetFLASHFile(QString);          //Ustaw œcie¿kê do pliku FLASH we zak³adkach
 void SetEEPROMFile(QString);         //Ustaw œcie¿kê do pliku EEPROM we wszystkich zak³adkach
 void SetSimplifierViewChBox(bool);   //Ustaw checkbox widoku uproszczonego w zak³adce Setup
 void SetAVRDudeWindowOnError(bool);  //Ustaw checkbox wyœwietlania okna AVRD Dude w przypadku b³êdu
 void SetAVRDudeCMDLine(bool);        //Ustaw checkbox wyœwietlania linii polecenia AVR Dude

private slots:
    void AVRDudeSetPath();            //Ustaw œcie¿kê do programu AVRDude
    void OpenFLASHFileDlg();          //Ustaw œcie¿kê do pliku z FLASH
    void OpenEEPROMFileDlg();         //Ustaw œcie¿kê do pliku z EEPROM
    //Z okienka SetUp
    void SetupShowSimplifiedView(int);     //Checkbox pokazuj¹cy tylko widok uproszczony
    void SetupShowAVDDudeWindow(int);      //Checkbox - poka¿ okno AVRDude tylko jeœli wyst¹pi³ b³¹d
    void SetupShowAVRDudeCmd(int);         //Poka¿ wygenerowane polecenie AVRDude

    //Programownaie
    void SavePathToAVRDude(QString);       //Zapisz œcie¿kê do AVRDude

    //Inne
    void SavePathToFLASHFile(QString);     //Zapisz w rejestrze œcie¿kê do pliku FLASH
    void SavePathToEEPROMFile(QString);    //Zapisz w rejestrze œcie¿kê do pliku EEPROM

private:
    Ui::MainDialog *ui;
};

#endif // MAINDIALOG_H
