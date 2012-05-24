#include "QProcessErrorMsg.h"

QProcessErrorMsg::QProcessErrorMsg(QProcess::ProcessError Error, QWidget * parent)
    :QMessageBox(parent)
{
    QString errtxt;
    switch(Error)
    {
    case QProcess::FailedToStart:	errtxt="B��d wywo�ania AVRDude. Plik programu nie istnieje, podana �cie�ka jest b��dna lub nie masz uprawnie� do jego wykonania.";
    case QProcess::Crashed:		    errtxt="AVRDude po uruchomieniu uleg� awarii. Zwykle znaczy to, �e co� jest naprawd� �le z plikiem AVRDude";
    case QProcess::Timedout:	    errtxt="Czas wykoanania AVRDude jest bardzo d�ugi. Mo�e si� zawiesi�?";
    case QProcess::WriteError:		errtxt="Problem z zapisem do strumienia wej�ciowego procesu.";
    case QProcess::ReadError:		errtxt="Problem z odczytem ze strumienia wyj�ciowego procesu.";
    case QProcess::UnknownError:	errtxt="Wszystko ok lub niezidentyfikowany problem :)";
    };

    setIcon(QMessageBox::Critical);
    addButton(QMessageBox::Ok);    //Przycisk Ok
    setText(errtxt);
    setWindowTitle("Problem z uruchomieniem AVRDude !!!");
}
