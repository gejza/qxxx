#ifndef XHASH_H
#define XHASH_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/qglobal.h>

class XHashData;

class XHash
{
public:
	XHash();
	XHash(const XHash &);
	XHash &operator=(const XHash &);
	~XHash();
	QString toString() const;
	QByteArray toByteArray() const;

	bool operator ==(const XHash& o) const;
	uint qHash(uint seed) const;

	static XHash fromFile(const QString& file_path);
	static XHash fromByteArray(const QByteArray& data);
private:
	QSharedDataPointer<XHashData> data;
};

inline uint qHash(const XHash& hash, uint seed) {
	return hash.qHash(seed);
}

#endif // XHASH_H
