#ifndef SIMPPGMDLG_H
#define SIMPPGMDLG_H

#include <QDialog>
#include "ui_simppgmdlg.h"

namespace Ui {
class SimpPgmDialog;
}

class SimpPgmDialog : public QDialog
{
    Q_OBJECT

public:
    enum Results {R_FLASH=1, R_EEPROM=2, R_FUSE=4, R_LOCK=8};  //Wyniki dialogu

    explicit SimpPgmDialog(QWidget *parent = 0);
    int GetResult();                 //Zwraca stan checkboxów

    ~SimpPgmDialog();

private slots:
    void Programm();                 //U¿ytkownik wybra³ programowanie

private:
    Ui::SimpPgmDialog *ui;
};

#endif // SIMPPGMDLG_H
