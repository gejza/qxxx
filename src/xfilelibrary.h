#ifndef XFILELIBRARY_H
#define XFILELIBRARY_H

#include <QObject>

class XHash;

class XFileLibrary : public QObject
{
	Q_OBJECT

	typedef QObject Super;
public:
	explicit XFileLibrary(QObject *parent = nullptr);

	static XHash getHash(const QString& file_path);
};

#endif // XFILELIBRARY_H
