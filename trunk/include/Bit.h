#ifndef BIT_H
#define BIT_H

#include <QMap>
#include <QVector>
#include "Value.h"

class Bit
{
public:
    virtual ~Bit() {}

    virtual QMap<QString, QString*> GetMappingMap();
    virtual void SetValues(QVector<Value> values);
//private:
    QString m_Caption;
    QString m_Mask;
    QString m_Name;
    QString m_Offset;
    QVector<Value> m_Values;
};

#endif // BIT_H
