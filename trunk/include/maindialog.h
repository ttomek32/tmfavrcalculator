#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <avrdudeconfparser.h>

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

protected:
    AvrdudeConfParser *AVRDudeConf;     //Parser pliku konfiguracyjnego AVRDude
    
public:
    explicit MainDialog(QWidget *parent = 0);
    void HideAdvancedTabs(bool);     //Ukryj zak�adki z opcjami zaawansowanymi

    ~MainDialog();

protected:
    void FillProgrammerCB();          //Wype�nij list� dost�pnych programator�w
    void FillPortCB();                //Wype�nij list� port�w
    void FillMCUType();               //Wype�nij list� mikrokontroler�w

signals:
 void SetAVRDudePath(QString);        //Ustaw �cie�k� w zak�adce SetUp do AVRDude
 void SetFLASHFile(QString);          //Ustaw �cie�k� do pliku FLASH we zak�adkach
 void SetEEPROMFile(QString);         //Ustaw �cie�k� do pliku EEPROM we wszystkich zak�adkach
 void SetSimplifierViewChBox(bool);   //Ustaw checkbox widoku uproszczonego w zak�adce Setup
 void SetAVRDudeWindowOnError(bool);  //Ustaw checkbox wy�wietlania okna AVRD Dude w przypadku b��du
 void SetAVRDudeCMDLine(bool);        //Ustaw checkbox wy�wietlania linii polecenia AVR Dude

private slots:
    //Z okienka programowanie uproszczone
    void ProgrammerChanged(QString);     //U�ytkownik wybra� typ programatora
    void PortChanged(QString);           //U�ytkownik wybra� typ portu
    void MCUChanged(QString);            //U�ytkownik wybra� typ mikrokontrolera
    void TestConnection();               //Sprawd� po��czenie z MCU i przy okazji sygnatur�

    void AVRDudeSetPath();            //Ustaw �cie�k� do programu AVRDude
    void OpenFLASHFileDlg();          //Ustaw �cie�k� do pliku z FLASH
    void OpenEEPROMFileDlg();         //Ustaw �cie�k� do pliku z EEPROM
    //Z okienka SetUp
    void SetupShowSimplifiedView(int);     //Checkbox pokazuj�cy tylko widok uproszczony
    void SetupShowAVDDudeWindow(int);      //Checkbox - poka� okno AVRDude tylko je�li wyst�pi� b��d
    void SetupShowAVRDudeCmd(int);         //Poka� wygenerowane polecenie AVRDude

    //Programownaie
    void SavePathToAVRDude(QString);       //Zapisz �cie�k� do AVRDude

    //Inne
    void SavePathToFLASHFile(QString);     //Zapisz w rejestrze �cie�k� do pliku FLASH
    void SavePathToEEPROMFile(QString);    //Zapisz w rejestrze �cie�k� do pliku EEPROM

private:
    Ui::MainDialog *ui;
};

#endif // MAINDIALOG_H
