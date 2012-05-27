#ifndef AVRDUDEERRORWINDOW_H
#define AVRDUDEERRORWINDOW_H

#include <QDialog>
#include "ui_AVRDudeErrorWindow.h"

namespace Ui {
class AVRDudeErrorWindow;
}

class AVRDudeErrorWindow : public QDialog
{
    Q_OBJECT

public:
    enum Results {R_FLASH=1, R_EEPROM=2, R_FUSE=4, R_LOCK=8};  //Wyniki dialogu

    explicit AVRDudeErrorWindow(QWidget *parent = 0);
    int GetResult();                 //Zwraca stan checkboxów

    ~AVRDudeErrorWindow();

private:
    Ui::AVRDudeErrorWindow *ui;
};

#endif // AVRDUDEERRORWINDOW_H
