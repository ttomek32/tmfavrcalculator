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
    explicit SimpPgmDialog(QWidget *parent = 0);

    ~SimpPgmDialog();

private slots:
    void Programm();                 //U¿ytkownik wybra³ programowanie

private:
    Ui::SimpPgmDialog *ui;
};

#endif // SIMPPGMDLG_H
