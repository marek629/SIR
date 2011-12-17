#include "error.h"

MetadataUtils::Error::Error() {}

MetadataUtils::Error::Error(Exiv2::Error &e) {
    copy(e);
}

MetadataUtils::Error::Error(const QString &message, Exiv2::Error &e) {
    setMessage(message);
    copy(e);
}

void MetadataUtils::Error::setMessage(const QString &str) {
    message_ = str;
}

void MetadataUtils::Error::copy(Exiv2::Error &e) {
    code_ = e.code();
    string = QString::fromUtf8(e.what());
}
