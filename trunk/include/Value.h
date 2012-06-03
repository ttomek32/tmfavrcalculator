#ifndef VALUE_H
#define VALUE_H

#define implements public

#include <QString>
#include <QMap>

class Value{
public:
    QMap<QString, QString*> GetMappingMap();

    QString GetName();
    QString GetCaption();
    QString GetValue();
private:
    QString m_Caption;
    QString m_Name;
    QString m_Value;
};

#endif // VALUE_H
