#include <QtGui/QApplication>
#include <QSplashScreen>
#include "maindialog.h"
#include <QTextCodec>
#include "delay.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":/resources/images/TmfAvrCalculator_SP_05_anim.gif");  //Przygotowane na ewentualny splash screen z reklamami
    QSplashScreen *splash = new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint);
    splash->show();
    qApp->processEvents();

    QCoreApplication::setOrganizationName("TMF");
    QCoreApplication::setOrganizationDomain("http://mikrokontrolery.blogspot.com/");
    QCoreApplication::setApplicationName("TMFAVRCalculator");

    QTextCodec::setCodecForTr (QTextCodec::codecForName("UTF-8")); //Ustaw kodowanie dla ca≥ej aplikacji. èrÛd≥a muszπ byÊ napisane w UTF-8

    //Delay::sleep(3);

    MainDialog w;
    w.show();


    delete splash;

    return a.exec();
}
