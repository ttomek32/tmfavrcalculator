#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include <QString>

class Programmer
{
public:
    Programmer();
    Programmer(
            QString id,
            QString description,
            QString type);

    QString GetID();
    QString GetDescription();
    QString GetType();
private:
    QString m_ID;
    QString m_Description;
    QString m_Type;
};

#endif // PROGRAMMER_H
