#include "videofile.h"

#include <QFileInfo>
#include <QJsonObject>
#include <QDateTime>


VideoFile::VideoFile()
{

}

VideoFile::VideoFile(int rowId)
{
	m_rowId = rowId;
}

VideoFile::~VideoFile()
{

}

int VideoFile::rowId() const
{
	return m_rowId;
}

QString VideoFile::filePath() const
{
	return m_path;
}

void VideoFile::setPath(const QString &path)
{
	m_path = path;
}

QDateTime VideoFile::createdTime() const
{
	return fi().lastModified();
}

QString VideoFile::fileName() const
{
	return fi().fileName();
}

QJsonObject VideoFile::toJson() const
{
	QJsonObject ret;
	ret["path"] = filePath();
	if (stars()) {
		ret["stars"] = stars();
	}
	return ret;
}

QFileInfo VideoFile::fi() const
{
	return QFileInfo(filePath());
}

VideoFile* VideoFile::fromJson(const QJsonObject &obj, int rowId)
{
	VideoFile *f = new VideoFile(rowId);
	f->setPath(obj.value("path").toString());
	QJsonValue jsv = obj.value("stars");
	if (!jsv.isUndefined()) {
		f->m_stars = jsv.toInt();
	}
	return f;
}
