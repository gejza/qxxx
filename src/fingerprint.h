#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <QByteArray>

class Fingerprint
{
public:
	Fingerprint();
	bool isNull() const {return m_data.isNull();}
	QString toString() const;
	static Fingerprint fromFile(const QString& path);
public:
	QByteArray m_data;
};

#endif // FINGERPRINT_H
