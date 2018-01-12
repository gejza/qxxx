#ifndef VIDEOLIBRARY_H
#define VIDEOLIBRARY_H

#include <QMap>
#include <QObject>
#include <QVector>
#include "videofile.h"

class VideoLibrary : public QObject
{
	Q_OBJECT
public:
	typedef int RowID;
	explicit VideoLibrary(QObject *parent = nullptr);
	static VideoLibrary* instance();

	VideoFile* getVideo(int index) const {return m_vids.at(index);}
	RowID addVideo(VideoFile vid);
	RowID count() const {return m_vids.count();}

	QJsonArray toJson() const;
	void fromJson(const QJsonArray& arr);

	void savePlaylist();
	void loadPlaylist();

	void findRecursion(const QString &path, const QString &pattern);
	void find(const QString& path);

signals:
	void filesAdded(int b_id, int e_id);
public slots:
protected:
	int findPath(const QString& p);
private:
	QVector<VideoFile*> m_vids;
	QMap<QString, RowID> m_pathIndex;
};

#endif // VIDEOLIBRARY_H
