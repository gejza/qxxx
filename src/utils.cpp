#include "utils.h"

#include <QCryptographicHash>
#include <QElapsedTimer>
#include <QFile>
#include <QDebug>

Utils::Utils()
{

}

QByteArray Utils::genSha1(const QString &file_path)
{
	QFile f(file_path);
	if (!f.open(QIODevice::ReadOnly)) {
		return QByteArray();
	}
	QCryptographicHash hash(QCryptographicHash::Sha1);
	hash.addData(&f);
	return hash.result();
}

QByteArray Utils::fingerprint(const QString &file_path)
{
	QElapsedTimer tim;
	tim.start();
	QFile f(file_path);
	if (!f.open(QIODevice::ReadOnly)) {
		return QByteArray();
	}
	QCryptographicHash hash(QCryptographicHash::Sha1);
	qint64 pos = 0;
	while (!f.atEnd() && pos < f.size() && pos < (30*1024*1024)) {
		f.seek(pos);
		hash.addData(f.read(1024));
		pos += 3 * 1024 * 1024;
	}
	qInfo() << "Compute fingerprint for" << file_path << fileSizeToString(f.size()) << msToString(tim.elapsed());
	return hash.result();
}

QString Utils::fileSizeToString(qint64 size)
{
	char unit;
	const char *units [] = {" B", " kB", " MB", " GB"};
	for (unit=-1; (++unit<3) && (size>1023); size/=1024);
	return QString::number(size, 'f', 1) + units[unit];
}

QString Utils::msToString(qint64 msecs)
{
	if (msecs > 1000) {
		QString::number(msecs / 1000) + " s";
	}
	return QString::number(msecs) + " ms";
}
