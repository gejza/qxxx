#include "videofile.h"

#include <QFileInfo>
#include <QJsonObject>

class VideoFile::Data : public QSharedData
{
public:
	int rowId = -1;
	QString path;
	QFileInfo fi() const { return QFileInfo(path);}
};

VideoFile::VideoFile() : data(new VideoFile::Data)
{

}

VideoFile::VideoFile(int rowId) : data(new VideoFile::Data)
{
	data->rowId = rowId;
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

int VideoFile::rowId() const
{
	return data->rowId;
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

VideoFile VideoFile::fromJson(const QJsonObject &obj, int rowId)
{
	VideoFile f(rowId);
	f.setPath(obj.value("path").toString());
	return f;
}
