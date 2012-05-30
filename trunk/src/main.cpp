#include <QtGui/QApplication>
#include <QSplashScreen>
#include "maindialog.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //QPixmap pixmap(":/images/idom-splash.png");  //Przygotowane na ewentualny splash screen z reklamami
    //QSplashScreen *splash = new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint);
    //splash->show();
    //qApp->processEvents();

    QCoreApplication::setOrganizationName("TMF");
    QCoreApplication::setOrganizationDomain("http://mikrokontrolery.blogspot.com/");
    QCoreApplication::setApplicationName("TMFAVRCalculator");

    QTextCodec::setCodecForTr (QTextCodec::codecForName("UTF-8")); //Ustaw kodowanie dla ca≥ej aplikacji. èrÛd≥a muszπ byÊ napisane w UTF-8

    MainDialog w;
    w.show();


//    delete splash;

    return a.exec();
}
