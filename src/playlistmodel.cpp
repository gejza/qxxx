#include "playlistmodel.h"
#include "utils.h"
#include "videolibrary.h"
#include "xhash.h"

#include <QDateTime>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

PlaylistModel::PlaylistModel(VideoLibrary *lib, QObject *parent)
	: QAbstractTableModel(parent), m_lib(lib)
{
	connect(lib, &VideoLibrary::fileAdded, [this](){
		//beginInsertRows(QModelIndex(), first, last);
		//endInsertRows();
		beginResetModel();
		m_items.clear();
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
	case ColFinger:
		return tr("Finger");
	case ColThumb:
		return tr("Image");
	};
	return QVariant();
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
	// For list models only the root node (an invalid parent) should return the list's size. For all
	// other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
	if (parent.isValid())
		return 0;

	return m_items.count();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return ColCount;
}

void PlaylistModel::sort(int column, Qt::SortOrder order)
{
	qInfo() << "Sorting by" << column << order;
	emit layoutAboutToBeChanged(QList<QPersistentModelIndex>(), QAbstractTableModel::VerticalSortHint);
	switch (column) {
	case ColName:
		m_items.setSort([order](const VideoFile*a, const VideoFile* b){
			return modelColumnCompareFunc(order, a->fileName(), b->fileName());
		});
		break;
	case ColCreated:
		m_items.setSort([order](const VideoFile*a, const VideoFile* b){
			return modelColumnCompareFunc(order, a->createdTime(), b->createdTime());
		});
		break;
	default:
		break;
	}
	emit layoutChanged(QList<QPersistentModelIndex>(), QAbstractTableModel::VerticalSortHint);
}

bool PlaylistModel::canFetchMore(const QModelIndex &parent) const
{
	return m_items.count() != m_lib->count();
}

void PlaylistModel::fetchMore(const QModelIndex &parent)
{
	beginResetModel();
	m_items = m_lib->all();
	std::sort(m_items.begin(), m_items.end(), [](VideoFile* a, VideoFile* b){
		return a->fileName() < b->fileName();
	});
	endResetModel();
}

VideoFile* PlaylistModel::at(const QModelIndex &idx) const
{
	return at(idx.row());
}

VideoFile *PlaylistModel::at(int row) const
{
	return m_items.at(row);
}

QModelIndex PlaylistModel::find(VideoFile *video) const
{
	int row = m_items.indexOf(video);
	if (row >= 0) {
		return index(row, 0);
	}
	return QModelIndex();
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
		case ColFinger:
			return 0;//at(index)->metaData(VideoFile::MD_Bitrate).toInt();// paths().count();// >hash().toString();
		};
		break;
	case Qt::DisplayRole:
		return data(index, Qt::EditRole);
	case Qt::DecorationRole:
		switch (index.column()) {
		case ColThumb:
			return at(index)->thumb();
		};
		break;
	}
	return QVariant();
}
