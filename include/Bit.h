#ifndef BIT_H
#define BIT_H

#include <QMap>
#include <QVector>
#include "Value.h"

class Bit
{
public:
    QMap<QString, QString*> GetMappingMap();

    QString GetName();
    QString GetCaption();
    QString GetMask();
    QString GetOffset();
    QString GetRegisterName();

    QVector<Value> GetValues();
    void SetValues(QVector<Value> values);
private:
    QString m_Caption;
    QString m_Mask;
    QString m_Name;
    QString m_Offset;
    QString m_RegisterName;
    QVector<Value> m_Values;
};

#endif // BIT_H
