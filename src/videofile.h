#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include <QExplicitlySharedDataPointer>

class QJsonObject;

class VideoFile
{
	class Data;
public:
	VideoFile();
	VideoFile(int rowId);
	VideoFile(const VideoFile &);
	VideoFile &operator=(const VideoFile &);
	~VideoFile();

	int rowId() const;

	QString filePath() const;
	void setPath(const QString& path);

	QString fileName() const;
	QJsonObject toJson() const;
	static VideoFile fromJson(const QJsonObject& obj, int rowId);
private:
	QExplicitlySharedDataPointer<Data> data;
};

#endif // VIDEOFILE_H
