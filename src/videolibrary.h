#ifndef VIDEOLIBRARY_H
#define VIDEOLIBRARY_H

#include "xfilelibrary.h"
#include "videofile.h"
#include <QMap>
#include <QVector>

class VideoLibrary : public XFileLibrary
{
	Q_OBJECT

	typedef XFileLibrary Super;
public:
	explicit VideoLibrary(QObject *parent = nullptr);
	static VideoLibrary* instance();

	//RowID addVideo(VideoFile vid);
	int count() const {return m_vids.count();}

	VideoFile* getVideo(const QString& path);
	VideoFile* getVideo(const XHash& id);
	QList<VideoFile*> all() const;

	QJsonArray toJson() const;
	void fromJson(const QJsonArray& arr);

	void savePlaylist();
	void saveData();
	void loadPlaylist();

	void findRecursion(const QString &path, const QString &pattern);
	void find(const QString& path);
	void computeHashes();
signals:
	void fileAdded(VideoFile* file);
public slots:
protected:

	//int findPath(const QString& p);
private:
	QHash<XHash, VideoFile*> m_vids;
	//QMap<QString, XFile::ID> m_pathIndex;
};

#endif // VIDEOLIBRARY_H
