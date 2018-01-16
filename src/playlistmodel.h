#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QPointer>
#include "videofile.h"

class QJsonArray;
class VideoLibrary;

class PlaylistModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum { ColName, ColCreated, ColSize, /*ColFinger,*/ ColCount, };

	explicit PlaylistModel(VideoLibrary* lib, QObject *parent = nullptr);

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	VideoFile* at(const QModelIndex& idx) const;
private:
	QPointer<VideoLibrary> m_lib;
};

#endif // PLAYLISTMODEL_H
