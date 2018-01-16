#include "fingerprint.h"
#include "utils.h"

#include <QString>

Fingerprint::Fingerprint()
{

}

QString Fingerprint::toString() const
{
	return m_data.toBase64().mid(16);
}

Fingerprint Fingerprint::fromFile(const QString &path)
{
	Fingerprint ret;
	ret.m_data = Utils::fingerprint(path);
	return ret;
}
