#include "playlistmodel.h"
#include "videolibrary.h"

#include <QDateTime>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>

PlaylistModel::PlaylistModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	connect(VideoLibrary::instance(), &VideoLibrary::filesAdded, [this](int first, int last){
		//this->beginInsertRows(QModelIndex(), first, last);
		//endInsertRows();
		checkIndex();
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
	};
	return QVariant();
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
	// For list models only the root node (an invalid parent) should return the list's size. For all
	// other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
	if (parent.isValid())
		return 0;

	const_cast<PlaylistModel*>(this)->checkIndex();
	return m_p.count();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return ColCount;
}

VideoFile* PlaylistModel::at(const QModelIndex &idx) const
{
	return VideoLibrary::instance()->getVideo(rowToId(idx.row()));
}

int PlaylistModel::rowToId(int row) const
{
	return m_p.at(row);
}

int PlaylistModel::idToRow(int id) const
{
	for (int i=0;i < m_p.count(); i++) {
		if (m_p.at(i) == id) {
			return i;
		}
	}
	return -1;
}

void PlaylistModel::checkIndex()
{
	if (m_p.count() < VideoLibrary::instance()->count()) {
		beginResetModel();
		while (m_p.count() < VideoLibrary::instance()->count()) {
			m_p << m_p.count();
		}

		std::sort(m_p.begin(), m_p.end(), [](int a, int b){
			return VideoLibrary::instance()->getVideo(a)->fileName() < VideoLibrary::instance()->getVideo(b)->fileName();
		});
		endResetModel();
	}
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
		};
		break;
	case Qt::DisplayRole:
		return data(index, Qt::EditRole);
	}
	return QVariant();
}
