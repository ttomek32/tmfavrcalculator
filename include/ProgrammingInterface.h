#ifndef PROGRAMMINGINTERFACE_H
#define PROGRAMMINGINTERFACE_H

#include <QString>
#include <QMap>

class ProgrammingInterface
{
public:
    QMap<QString, QString*> GetMappingMap();

private:
    QString m_Name;
    QString m_Type;
};

#endif // PROGRAMMINGINTERFACE_H
