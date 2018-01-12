#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include <QExplicitlySharedDataPointer>

class QJsonObject;
class QFileInfo;
class QDateTime;

class VideoFile
{
	class Data;
public:
	VideoFile();
	VideoFile(int rowId);
	virtual ~VideoFile();

	int rowId() const;

	QString filePath() const;
	void setPath(const QString& path);

	int stars() const {return m_stars;}
	void addStar() {m_stars++;}

	QDateTime createdTime() const;

	QString fileName() const;
	QJsonObject toJson() const;
	static VideoFile* fromJson(const QJsonObject& obj, int rowId);
protected:
	QFileInfo fi() const;
private:
	Q_DISABLE_COPY(VideoFile)

	int m_rowId = -1;
	int m_stars = 0;
	QString m_path;
};

#endif // VIDEOFILE_H
