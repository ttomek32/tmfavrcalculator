#ifndef VALUE_H
#define VALUE_H

#define implements public

#include <QString>
#include <QMap>

class Value{
public:
    Value();

    QMap<QString, QString*> GetMappingMap();
private:
    QString m_Caption;
    QString m_Name;
    QString m_Value;
};

#endif // VALUE_H
