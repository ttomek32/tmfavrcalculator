#include <QtGui/QApplication>
#include <QSplashScreen>
#include "maindialog.h"

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

    MainDialog w;
    w.show();


//    delete splash;

    return a.exec();
}
