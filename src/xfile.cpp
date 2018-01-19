#include "db.h"
#include "xfile.h"
#include "xhash.h"

#include <QDateTime>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>

XFile::XFile(const XHash &id, QObject *parent)
	: Super(parent), m_id(id)
{
	//return XHash::fromFile(filePath());

}

QString XFile::filePath() const
{
	return m_path.isEmpty() ? QString() : *m_path.begin();
}

void XFile::addPath(const QString &path)
{
	if (!m_path.contains(path)) {
		if (!m_path.isEmpty()) {
			qInfo() << "Add second path" << path << filePath();
		}
		m_path << path;
	}
}

QDateTime XFile::createdTime() const
{
	return fi().lastModified();
}

qint64 XFile::fileSize() const
{
	return fi().size();
}

QString XFile::fileName() const
{
	return fi().fileName();
}

QJsonObject XFile::toJson() const
{
	QJsonObject ret;
	ret["id"] = QString::fromLatin1(id().toByteArray().toBase64());
	ret["path"] = filePath();
	QJsonArray paths;
	for (QString p : m_path) {
		paths.append(p);
	}
	ret["paths"] = paths;
	return ret;
}

void XFile::loadJson(const QJsonObject &json)
{
	addPath(json.value("path").toString());
	QJsonValue jsv = json.value("paths");
	if (jsv.isArray()) {
		for (auto p : jsv.toArray()) {
			addPath(p.toString());
		}
	}
}

void XFile::save()
{
	if (m_changed) {
		QJsonDocument doc(toJson());
		XDb::instance()->put(id().toByteArray(), doc.toBinaryData());
		Q_FOREACH(QString p, m_path) {
			XDb::instance()->setPath(p, id());
		}
		m_changed = false;
	}
}

QFileInfo XFile::fi() const
{
	return QFileInfo(filePath());
}
