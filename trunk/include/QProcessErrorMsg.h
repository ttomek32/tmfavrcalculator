#ifndef QPROCESSERRORMSG_H
#define QPROCESSERRORMSG_H

#include <QMessageBox>
#include <QProcess>

class QProcessErrorMsg : public QMessageBox
{
public:
    QProcessErrorMsg(QProcess::ProcessError Error, QWidget * parent = 0);
};

#endif // QPROCESSERRORMSG_H
