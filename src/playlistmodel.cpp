#include "playlistmodel.h"
#include "utils.h"
#include "videolibrary.h"

#include <QDateTime>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>

PlaylistModel::PlaylistModel(VideoLibrary *lib, QObject *parent)
	: QAbstractTableModel(parent), m_lib(lib)
{
	connect(lib, &VideoLibrary::filesAdded, [this](int first, int last){
		//beginInsertRows(QModelIndex(), first, last);
		//endInsertRows();
		beginResetModel();
		endResetModel();
	});
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
		return  QVariant();
	}
	switch (section) {
	case ColName:
		return tr("Name");
	case ColCreated:
		return tr("Created");
	case ColSize:
		return tr("Size");
	//case ColFinger:
	//	return tr("Finger");
	};
	return QVariant();
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
	// For list models only the root node (an invalid parent) should return the list's size. For all
	// other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
	if (parent.isValid())
		return 0;

	return m_lib.isNull() ? 0 : m_lib->count();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return ColCount;
}

VideoFile* PlaylistModel::at(const QModelIndex &idx) const
{
	Q_ASSERT(!m_lib.isNull());
	return m_lib->getVideo(idx.row());
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	switch (role) {
	case Qt::EditRole:
		switch (index.column()) {
		case ColName:
			return at(index)->fileName();
		case ColCreated:
			return at(index)->createdTime().toString();
		case ColSize:
			return Utils::fileSizeToString(at(index)->fileSize());
		//case ColFinger:
		//	return at(index)->fingerprint().toString();
		};
		break;
	case Qt::DisplayRole:
		return data(index, Qt::EditRole);
	}
	return QVariant();
}
