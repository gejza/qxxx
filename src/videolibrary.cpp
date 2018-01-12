#include "preferences.h"
#include "videolibrary.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSaveFile>

VideoLibrary::VideoLibrary(QObject *parent) : QObject(parent)
{

}

VideoLibrary *VideoLibrary::instance()
{
	static VideoLibrary lib;
	return &lib;
}

QJsonArray VideoLibrary::toJson() const
{
	QJsonArray arr;
	for (int i=0;i < m_vids.count();i++) {
			arr.append(m_vids.at(i).toJson());
	}
	return arr;
}

void VideoLibrary::fromJson(const QJsonArray &arr)
{
	int bc = m_vids.count();
	for (auto it=arr.constBegin(); it != arr.constEnd(); ++it) {
		m_vids << VideoFile::fromJson(it->toObject(), m_vids.count()-1);
	}
	if (bc != m_vids.count()) {
		emit filesAdded(bc-1, m_vids.count());
	}
}

void VideoLibrary::savePlaylist()
{
	QSaveFile f(Preferences::instance()->dbPath());
	QJsonDocument doc(toJson());
	if (f.open(QIODevice::WriteOnly)) {
		f.write(doc.toJson());
		f.commit();
	}
}

void VideoLibrary::loadPlaylist()
{
	QFile f("playlist.json");
	if (f.open(QIODevice::ReadOnly)) {
		auto doc = QJsonDocument::fromJson(f.readAll());
		fromJson(doc.array());
	}
}

void VideoLibrary::findRecursion(const QString &path, const QString &pattern)
{
	QDir currentDir(path);
	const QString prefix = path + QLatin1Char('/');
	foreach (const QString &match, currentDir.entryList(QStringList(pattern), QDir::Files | QDir::NoSymLinks)) {
		VideoFile v(m_vids.count()-1);
		v.setPath(prefix + match);
		m_vids << v;
	}
	foreach (const QString &dir, currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
		findRecursion(prefix + dir, pattern);
}

void VideoLibrary::find(const QString &path)
{
	int bc = m_vids.count();
	findRecursion(path, "*.*");
	if (bc != m_vids.count()) {
		emit filesAdded(bc-1, m_vids.count());
	}

}

int VideoLibrary::findPath(const QString &p)
{
	while (m_pathIndex.count() < m_vids.count()) {
		int rid = m_pathIndex.count();
		m_pathIndex.insert(getVideo(rid).filePath(), rid);
	}
	return m_pathIndex.value(p, -1);
}
