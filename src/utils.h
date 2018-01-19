#ifndef UTILS_H
#define UTILS_H

#include <qglobal.h>

class Utils
{
public:
	Utils();

	static QByteArray genSha1(const QString& file_path);
	static QByteArray fingerprint(const QString& file_path);

	static QString fileSizeToString(qint64 size);
	static QString msToString(qint64 msecs);
};

#endif // UTILS_H
