#ifndef CONFIGPARSEEXCEPTION_H
#define CONFIGPARSEEXCEPTION_H

#include <QtCore>

class ConfigParseException : public QtConcurrent::Exception
{
public:
    void raise() const
    {
        throw *this;
    }

    Exception *clone() const
    {
        return new ConfigParseException(*this);
    }
};

#endif // CONFIGPARSEEXCEPTION_H
