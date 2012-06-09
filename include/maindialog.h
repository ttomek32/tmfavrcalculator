#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QPalette>
#include "AVRFactory.h"

//Pliki s¹ zale¿ne czy kompilujemy w Windows czy Linux
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
    QString LastSelFuseByte;            //Nazwa ostatnio wybranego pola edycji fusebitów
    QPalette editpal;                   //Paleta kolorów okienek edycji fusebitów
    
public:
    explicit MainDialog(QWidget *parent = 0);
    void HideAdvancedTabs(bool);     //Ukryj zak³adki z opcjami zaawansowanymi

    ~MainDialog();

protected:
    void FillProgrammerCB();          //Wype³nij listê dostêpnych programatorów
    void FillPortCB();                //Wype³nij listê portów
    void FillMCUType();               //Wype³nij listê mikrokontrolerów

    uint8_t HowManyFuseBytes();       //Zwraca liczbê fusebajtów w wybranym mikrokontrolerze
    void UpdateFuseBitsWidget();      //Uaktualnij zak³adkê fusebitów po zmianie MCU
    void UpdateLockBitsWidget();      //Uaktualnij zak³adkê lockbitów po zmianie MCU
    void UpdateLockBitTable(uint8_t val); //Uaktualnij obiekt QTableWidget wyœwietlaj¹cy ustawienia lockbitów
    void UpdateLockByteCheckboxes(uint8_t val); //Uaktualnij checkboxy lockbitów

signals:
 void SetAVRDudePath(QString);        //Ustaw œcie¿kê w zak³adce SetUp do AVRDude
 void SetBinutilsPath(QString);       //Ustaw œcie¿kê w zak³adce SetUp do binutils
 void SetFLASHFile(QString);          //Ustaw œcie¿kê do pliku FLASH we zak³adkach
 void SetEEPROMFile(QString);         //Ustaw œcie¿kê do pliku EEPROM we wszystkich zak³adkach
 void SetSimplifierViewChBox(bool);   //Ustaw checkbox widoku uproszczonego w zak³adce Setup
 void SetAVRDudeWindowOnError(bool);  //Ustaw checkbox wyœwietlania okna AVRDude w przypadku b³êdu
 void SetAVRDudeCMDLine(bool);        //Ustaw checkbox wyœwietlania linii polecenia AVR Dude

private slots:
    //Z okienka programowanie uproszczone
    void ProgrammerChanged(QString);     //U¿ytkownik wybra³ typ programatora
    void PortChanged(QString);           //U¿ytkownik wybra³ typ portu
    void MCUChanged(QString);            //U¿ytkownik wybra³ typ mikrokontrolera
    void TestConnection();               //SprawdŸ po³¹czenie z MCU i przy okazji sygnaturê
    void ProgrammBtn();                  //Wciœniêto przycisk programowanie

    void AVRDudeSetPath();            //Ustaw œcie¿kê do programu AVRDude
    void OpenFLASHFileDlg();          //Ustaw œcie¿kê do pliku z FLASH
    void OpenEEPROMFileDlg();         //Ustaw œcie¿kê do pliku z EEPROM

    //Z okienka SetUp
    void SetupShowSimplifiedView(int);     //Checkbox pokazuj¹cy tylko widok uproszczony
    void SetupShowAVDDudeWindow(int);      //Combobox - poka¿ okno AVRDude tylko jeœli wyst¹pi³ b³¹d
    void SetupShowAVRDudeCmd(int);         //Poka¿ wygenerowane polecenie AVRDude
    void SavePathToAVRBinutils(QString);   //Zapisz œcie¿kê do narzêdzi binutils
    void AVRBinutilsSetPath();             //Otwórz okno wyboru œcie¿ki do binutls
    void SavePathToAVRDude(QString);       //Zapisz œcie¿kê do AVRDude

    //Programownaie
    void EraseFLASHChBox(int);             //Przycisk kasowania CPU
    void VerifyFLASHChBox(int);            //Przycisk weryfikacji pamiêci
    void ReadFLASH();                      //Odczytaj pamiêæ FLASH i zapisz do pliku
    void WriteFLASH();                     //Zapisz pamiêæ FLASH
    void VerifyFLASH();                    //Porównaj FLASH MCU z plikiem
    void ReadEEPROM();                     //Odczytaj pamiêæ EEPROM i zapisz do pliku
    void WriteEEPROM();                    //Zapisz pamiêæ EEPROM
    void VerifyEEPROM();                   //Porównaj EEPROM MCU z plikiem

    //Z okienka fusebity
    void FuseByteChanged();                //W jednym z pól edycyjnych zmieni³a siê wartoœæ
    void BitChBoxChg(int);                 //Stan checkboxa zwi¹zanego z fusebitami uleg³ zmianie
    void ReadFuse();                       //Odczytaj pamiêæ FLASH i zapisz do pliku
    void WriteFuse();                      //Zapisz pamiêæ FLASH
    void VerifyFuse();                     //Porównaj FLASH MCU z plikiem
    void DefaultFuse();                    //Ustaw domyœlne fusebity (bez programowania)
    void EnableFuseBytes();                //Sprawdza które fusebajty s¹ dostêpne i blokuje niedostêpne

    //Z okienka lockbity
    void ReadLock();                       //Odczytaj pamiêæ Lock i zapisz do pliku
    void WriteLock();                      //Zapisz pamiêæ Lock
    void VerifyLock();                     //Porównaj Lock MCU z plikiem
    void LockBitChBoxChg();                //U¿ytkownik zmieni³ stan checkboxów zwi¹zanych z lockbitami
    void LockByteChanged();                //U¿ytkownik zmieni³ pole okreœlaj¹ce wartoœæ hex lickbitów

    //Inne
    void SavePathToFLASHFile(QString);     //Zapisz w rejestrze œcie¿kê do pliku FLASH
    void SavePathToEEPROMFile(QString);    //Zapisz w rejestrze œcie¿kê do pliku EEPROM

    void ShowAVRDudeCmdLineParams();       //Wyœwietla na dole w okienku liniê parametrów AVRDude
    void EnablePgmButtons();               //Sprawdza czy mo¿na odblokowaæ przyciski programowania (read, verify, write) we wszystkich zak³adkach, jeœli tak, to je odblokowuje
    void DisableEEPROMIfElf(QString);      //Disable EEPROM dialogs if using elf file

    void FuseBitChangedByUser();           //U¿ytkownik zmieni³ ustawienia fusebitów w widoku tabeli
    void LockBitChangedByUser();           //Lockbit zosta³ zmieniony przez u¿ytkownika w widoku tabeli.

private:
    Ui::MainDialog *ui;

  //Funkcje zwracaj¹ce poszczególne elementy linii polecenia AVRDude
    QString GetMCUAsAVRDudeParam();         //Zwróæ typ MCU jako parametr
    QString GetProgrammerAsAVRDudeParam();  //Zwróæ typ programatora jako parametr
    QString GetPortAsAVRDudeParam();        //Zwróæ typ wybranego portu jako parametr
    QString GetPerformEraseChipAsAVRDudeParam(); //Zwróæ parametr - kasowanie chipu w zale¿noœci od ustawienia checkboxa
    QString GetFLASHFilePath();             //Zwróæ œcie¿kê + nazwê pliku z zawartoœci¹ FLASH
    QString GetEEPROMFilePath();            //Zwróæ œcie¿kê + nazwê pliku z zawartoœci¹ EEPROM
    QString GetBinutilsPath();              //Zwróæ œcie¿kê do binutils lub pusty ³añcuch jeœli jej nie ma lub nie ma plików avr-objcopy i avr-objdump
    void AVRDudeCmdLineParams();            //Wyœwietla na dole w okienku liniê parametrów AVRDude
    void GetLockBitsAVRDudeCmdParams(QStringList *); //Zwróæ listê parametrów AVRDude dla odczytu bie¿¹cej konfiguracji lockbitów
    void SetLockBitsAVRDudeCmdParams(QStringList *); //Zwróæ listê parametrów AVRDude dla zapisu bie¿¹cej konfiguracji lockbitów
    void GetFuseBitsAVRDudeCmdParams(QStringList *); //Zwróæ listê parametrów AVRDude dla odczytu bie¿¹cej konfiguracji fusebitów
    void SetFuseBitsAVRDudeCmdParams(QStringList *); //Zwróæ listê parametrów AVRDude dla zapisu bie¿¹cej konfiguracji fusebitów
    QString GetSupportedFileExtensions();   //Zwraca listê wspieranych rozszerzeñ plików do dialogów open file
};

#endif // MAINDIALOG_H
