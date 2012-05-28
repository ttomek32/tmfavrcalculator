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
    bool fin;

    explicit AVRDudeErrorWindow(QWidget *parent = 0);
    ~AVRDudeErrorWindow();

    Ui::AVRDudeErrorWindow *ui;

private slots:
    virtual void reject();
};

#endif // AVRDUDEERRORWINDOW_H
