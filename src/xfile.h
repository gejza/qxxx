#ifndef XFILE_H
#define XFILE_H

#include <QObject>
#include <QSet>
#include "xhash.h"

class QFileInfo;
class QDateTime;

class XFile : public QObject
{
	Q_OBJECT

	typedef QObject Super;
public:
	enum MediaType {
		Video = 1,
		Image = 2,
	};

	explicit XFile(const XHash& id, QObject *parent = nullptr);
	XHash id() const {return m_id;}

	QString filePath() const;
	void addPath(const QString& path);
	bool hasPath(const QString& path) const {return m_path.contains(path);}
	QStringList paths() const {return m_path.toList();}
	QDateTime createdTime() const;
	qint64 fileSize() const;
	QString fileName() const;

	virtual QJsonObject toJson() const;
	virtual void loadJson(const QJsonObject& json);
	//static VideoFile* fromJson(const QJsonObject& obj, int rowId);

	void save();
protected:
	QFileInfo fi() const;
private:
	Q_DISABLE_COPY(XFile)
	XHash m_id;
	bool m_changed = false;
	QSet<QString> m_path;
};

#endif // XFILE_H
