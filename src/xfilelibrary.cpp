#include "db.h"
#include "xfilelibrary.h"
#include "xhash.h"

#include <QDebug>

XFileLibrary::XFileLibrary(QObject *parent)
	: Super(parent)
{

}

XHash XFileLibrary::getHash(const QString &file_path)
{
	QByteArray h = XDb::instance()->getPath(file_path);
	if (!h.isEmpty()) {
		qDebug() << "Using" << file_path << "from db";
		return XHash::fromByteArray(h);
	}
	XHash ret = XHash::fromFile(file_path);
	XDb::instance()->setPath(file_path, ret);
	return ret;
}
