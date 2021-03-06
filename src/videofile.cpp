#include "videofile.h"

#include <QFileInfo>
#include <QJsonObject>

class VideoFile::Data : public QSharedData
{
public:
	QString path;
	QFileInfo fi() const { return QFileInfo(path);}
};

VideoFile::VideoFile() : data(new VideoFile::Data)
{

}

VideoFile::VideoFile(const VideoFile &rhs) : data(rhs.data)
{

}

VideoFile &VideoFile::operator=(const VideoFile &rhs)
{
	if (this != &rhs)
		data.operator=(rhs.data);
	return *this;
}

VideoFile::~VideoFile()
{

}

QString VideoFile::filePath() const
{
	return data->path;
}

void VideoFile::setPath(const QString &path)
{
	data->path = path;
}

QString VideoFile::fileName() const
{
	return data->fi().fileName();
}

QJsonObject VideoFile::toJson() const
{
	QJsonObject ret;
	ret["path"] = filePath();
	return ret;
}

VideoFile VideoFile::fromJson(const QJsonObject &obj)
{
	VideoFile f;
	f.setPath(obj.value("path").toString());
	return f;
}
