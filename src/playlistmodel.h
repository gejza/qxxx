#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include "videofile.h"

class QJsonArray;

class PlaylistModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum { ColName, ColCount, };

	explicit PlaylistModel(QObject *parent = nullptr);

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	VideoFile at(const QModelIndex& idx) const;
	int rowToId(int row) const;
	int idToRow(int id) const;
protected:
	void checkIndex();
private:
	mutable QVector<int> m_p;
};

#endif // PLAYLISTMODEL_H
