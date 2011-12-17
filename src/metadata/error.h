#ifndef ERROR_H
#define ERROR_H

#include <exiv2/error.hpp>
#include <QString>

namespace MetadataUtils
{
    class Error
    {
    public:
        Error();
        Error(Exiv2::Error &e);
        Error(const QString &message, Exiv2::Error &e);
        void copy(Exiv2::Error &e);
        void setMessage(const QString &str);
        const QString &message() const { return message_; }
        const QString &what() const { return string; }
        int code() const { return code_; }
    private:
        QString message_;
        QString string;
        int code_;
    };
}

#endif // ERROR_H
