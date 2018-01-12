#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include <QSharedDataPointer>

class QJsonObject;

class VideoFile
{
	class Data;
public:
	VideoFile();
	VideoFile(const VideoFile &);
	VideoFile &operator=(const VideoFile &);
	~VideoFile();

	QString filePath() const;
	void setPath(const QString& path);

	QString fileName() const;
	QJsonObject toJson() const;
	static VideoFile fromJson(const QJsonObject& obj);
private:
	QSharedDataPointer<Data> data;
};

#endif // VIDEOFILE_H
