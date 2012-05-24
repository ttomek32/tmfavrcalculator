#include "QProcessErrorMsg.h"

QProcessErrorMsg::QProcessErrorMsg(QProcess::ProcessError Error, QWidget * parent)
    :QMessageBox(parent)
{
    QString errtxt;
    switch(Error)
    {
    case QProcess::FailedToStart:	errtxt="B³¹d wywo³ania AVRDude. Plik programu nie istnieje, podana œcie¿ka jest b³êdna lub nie masz uprawnieñ do jego wykonania.";
    case QProcess::Crashed:		    errtxt="AVRDude po uruchomieniu uleg³ awarii. Zwykle znaczy to, ¿e coœ jest naprawdê Ÿle z plikiem AVRDude";
    case QProcess::Timedout:	    errtxt="Czas wykoanania AVRDude jest bardzo d³ugi. Mo¿e siê zawiesi³?";
    case QProcess::WriteError:		errtxt="Problem z zapisem do strumienia wejœciowego procesu.";
    case QProcess::ReadError:		errtxt="Problem z odczytem ze strumienia wyjœciowego procesu.";
    case QProcess::UnknownError:	errtxt="Wszystko ok lub niezidentyfikowany problem :)";
    };

    setIcon(QMessageBox::Critical);
    addButton(QMessageBox::Ok);    //Przycisk Ok
    setText(errtxt);
    setWindowTitle("Problem z uruchomieniem AVRDude !!!");
}
