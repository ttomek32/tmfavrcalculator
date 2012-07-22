#include <QtGui/QApplication>
#include "maindialog.h"
#include <QTextCodec>
#include "delay.h"
#include "splash.h"

QSplashScreen *splash;  //Splash screen - dostêp do niego jest potrzebny w innych czêœciach aplikacji

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":/resources/images/SplashScreen_1.png");  //Przygotowane na ewentualny splash screen z reklamami
    splash = new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint);
    splash->show();
    qApp->processEvents();

    QCoreApplication::setOrganizationName("TMF");
    QCoreApplication::setOrganizationDomain("http://mikrokontrolery.blogspot.com/");
    QCoreApplication::setApplicationName("TMFAVRCalculator");

    QTextCodec::setCodecForTr (QTextCodec::codecForName("UTF-8")); //Ustaw kodowanie dla ca³ej aplikacji. ród³a musz¹ byæ napisane w UTF-8

    //Delay::sleep(3);

    MainDialog w;
    splash->finish(&w);
    w.show();

    return a.exec();
}
