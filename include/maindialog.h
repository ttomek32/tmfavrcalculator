#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QPalette>
#include "AVRFactory.h"

//Pliki s� zale�ne czy kompilujemy w Windows czy Linux
#ifdef Q_OS_UNIX
#define AVROBJCOPY  "avr-objcopy"
#define AVROBJDUMP  "avr-objdump"
#endif

#ifdef Q_OS_WIN
#define AVROBJCOPY  "avr-objcopy.exe"
#define AVROBJDUMP  "avr-objdump.exe"
#endif

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

protected:
    AVRFactory *AVR;                    //Komplet informacji o programatorach i wspieranych mikrokontrolerach
    QString LastSelFuseByte;            //Nazwa ostatnio wybranego pola edycji fusebit�w
    QPalette editpal;                   //Paleta kolor�w okienek edycji fusebit�w
    
public:
    explicit MainDialog(QWidget *parent = 0);
    void HideAdvancedTabs(bool);     //Ukryj zak�adki z opcjami zaawansowanymi

    ~MainDialog();

protected:
    void FillProgrammerCB();          //Wype�nij list� dost�pnych programator�w
    void FillPortCB();                //Wype�nij list� port�w
    void FillMCUType();               //Wype�nij list� mikrokontroler�w

    uint8_t HowManyFuseBytes();       //Zwraca liczb� fusebajt�w w wybranym mikrokontrolerze
    void UpdateFuseBitsWidget();      //Uaktualnij zak�adk� fusebit�w po zmianie MCU
    void UpdateLockBitsWidget();      //Uaktualnij zak�adk� lockbit�w po zmianie MCU
    void UpdateLockBitTable(uint8_t val); //Uaktualnij obiekt QTableWidget wy�wietlaj�cy ustawienia lockbit�w
    void UpdateLockByteCheckboxes(uint8_t val); //Uaktualnij checkboxy lockbit�w

signals:
 void SetAVRDudePath(QString);        //Ustaw �cie�k� w zak�adce SetUp do AVRDude
 void SetBinutilsPath(QString);       //Ustaw �cie�k� w zak�adce SetUp do binutils
 void SetFLASHFile(QString);          //Ustaw �cie�k� do pliku FLASH we zak�adkach
 void SetEEPROMFile(QString);         //Ustaw �cie�k� do pliku EEPROM we wszystkich zak�adkach
 void SetSimplifierViewChBox(bool);   //Ustaw checkbox widoku uproszczonego w zak�adce Setup
 void SetAVRDudeWindowOnError(bool);  //Ustaw checkbox wy�wietlania okna AVRDude w przypadku b��du
 void SetAVRDudeCMDLine(bool);        //Ustaw checkbox wy�wietlania linii polecenia AVR Dude

private slots:
    //Z okienka programowanie uproszczone
    void ProgrammerChanged(QString);     //U�ytkownik wybra� typ programatora
    void PortChanged(QString);           //U�ytkownik wybra� typ portu
    void MCUChanged(QString);            //U�ytkownik wybra� typ mikrokontrolera
    void TestConnection();               //Sprawd� po��czenie z MCU i przy okazji sygnatur�
    void ProgrammBtn();                  //Wci�ni�to przycisk programowanie

    void AVRDudeSetPath();            //Ustaw �cie�k� do programu AVRDude
    void OpenFLASHFileDlg();          //Ustaw �cie�k� do pliku z FLASH
    void OpenEEPROMFileDlg();         //Ustaw �cie�k� do pliku z EEPROM

    //Z okienka SetUp
    void SetupShowSimplifiedView(int);     //Checkbox pokazuj�cy tylko widok uproszczony
    void SetupShowAVDDudeWindow(int);      //Combobox - poka� okno AVRDude tylko je�li wyst�pi� b��d
    void SetupShowAVRDudeCmd(int);         //Poka� wygenerowane polecenie AVRDude
    void SavePathToAVRBinutils(QString);   //Zapisz �cie�k� do narz�dzi binutils
    void AVRBinutilsSetPath();             //Otw�rz okno wyboru �cie�ki do binutls
    void SavePathToAVRDude(QString);       //Zapisz �cie�k� do AVRDude

    //Programownaie
    void EraseFLASHChBox(int);             //Przycisk kasowania CPU
    void VerifyFLASHChBox(int);            //Przycisk weryfikacji pami�ci
    void ReadFLASH();                      //Odczytaj pami�� FLASH i zapisz do pliku
    void WriteFLASH();                     //Zapisz pami�� FLASH
    void VerifyFLASH();                    //Por�wnaj FLASH MCU z plikiem
    void ReadEEPROM();                     //Odczytaj pami�� EEPROM i zapisz do pliku
    void WriteEEPROM();                    //Zapisz pami�� EEPROM
    void VerifyEEPROM();                   //Por�wnaj EEPROM MCU z plikiem

    //Z okienka fusebity
    void FuseByteChanged();                //W jednym z p�l edycyjnych zmieni�a si� warto��
    void BitChBoxChg(int);                 //Stan checkboxa zwi�zanego z fusebitami uleg� zmianie
    void ReadFuse();                       //Odczytaj pami�� FLASH i zapisz do pliku
    void WriteFuse();                      //Zapisz pami�� FLASH
    void VerifyFuse();                     //Por�wnaj FLASH MCU z plikiem
    void DefaultFuse();                    //Ustaw domy�lne fusebity (bez programowania)
    void EnableFuseBytes();                //Sprawdza kt�re fusebajty s� dost�pne i blokuje niedost�pne

    //Z okienka lockbity
    void ReadLock();                       //Odczytaj pami�� Lock i zapisz do pliku
    void WriteLock();                      //Zapisz pami�� Lock
    void VerifyLock();                     //Por�wnaj Lock MCU z plikiem
    void LockBitChBoxChg();                //U�ytkownik zmieni� stan checkbox�w zwi�zanych z lockbitami
    void LockByteChanged();                //U�ytkownik zmieni� pole okre�laj�ce warto�� hex lickbit�w

    //Inne
    void SavePathToFLASHFile(QString);     //Zapisz w rejestrze �cie�k� do pliku FLASH
    void SavePathToEEPROMFile(QString);    //Zapisz w rejestrze �cie�k� do pliku EEPROM

    void ShowAVRDudeCmdLineParams();       //Wy�wietla na dole w okienku lini� parametr�w AVRDude
    void EnablePgmButtons();               //Sprawdza czy mo�na odblokowa� przyciski programowania (read, verify, write) we wszystkich zak�adkach, je�li tak, to je odblokowuje
    void DisableEEPROMIfElf(QString);      //Disable EEPROM dialogs if using elf file

    void FuseBitChangedByUser();           //U�ytkownik zmieni� ustawienia fusebit�w w widoku tabeli
    void LockBitChangedByUser();           //Lockbit zosta� zmieniony przez u�ytkownika w widoku tabeli.

private:
    Ui::MainDialog *ui;

  //Funkcje zwracaj�ce poszczeg�lne elementy linii polecenia AVRDude
    QString GetMCUAsAVRDudeParam();         //Zwr�� typ MCU jako parametr
    QString GetProgrammerAsAVRDudeParam();  //Zwr�� typ programatora jako parametr
    QString GetPortAsAVRDudeParam();        //Zwr�� typ wybranego portu jako parametr
    QString GetPerformEraseChipAsAVRDudeParam(); //Zwr�� parametr - kasowanie chipu w zale�no�ci od ustawienia checkboxa
    QString GetFLASHFilePath();             //Zwr�� �cie�k� + nazw� pliku z zawarto�ci� FLASH
    QString GetEEPROMFilePath();            //Zwr�� �cie�k� + nazw� pliku z zawarto�ci� EEPROM
    QString GetBinutilsPath();              //Zwr�� �cie�k� do binutils lub pusty �a�cuch je�li jej nie ma lub nie ma plik�w avr-objcopy i avr-objdump
    void AVRDudeCmdLineParams();            //Wy�wietla na dole w okienku lini� parametr�w AVRDude
    void GetLockBitsAVRDudeCmdParams(QStringList *); //Zwr�� list� parametr�w AVRDude dla odczytu bie��cej konfiguracji lockbit�w
    void SetLockBitsAVRDudeCmdParams(QStringList *); //Zwr�� list� parametr�w AVRDude dla zapisu bie��cej konfiguracji lockbit�w
    void GetFuseBitsAVRDudeCmdParams(QStringList *); //Zwr�� list� parametr�w AVRDude dla odczytu bie��cej konfiguracji fusebit�w
    void SetFuseBitsAVRDudeCmdParams(QStringList *); //Zwr�� list� parametr�w AVRDude dla zapisu bie��cej konfiguracji fusebit�w
    QString GetSupportedFileExtensions();   //Zwraca list� wspieranych rozszerze� plik�w do dialog�w open file
};

#endif // MAINDIALOG_H
