#include "playlistmodel.h"

#include <QDir>
#include <QJsonArray>
#include <QJsonObject>

PlaylistModel::PlaylistModel(QObject *parent)
	: QAbstractTableModel(parent)
{
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal) {
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

	return m_vids.count();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return ColCount;
}

void PlaylistModel::findRecursion(const QString &path, const QString &pattern)
{
	QDir currentDir(path);
	const QString prefix = path + QLatin1Char('/');
	foreach (const QString &match, currentDir.entryList(QStringList(pattern), QDir::Files | QDir::NoSymLinks)) {
		VideoFile v;
		v.setPath(prefix + match);
		m_vids << v;
	}
	foreach (const QString &dir, currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
		findRecursion(prefix + dir, pattern);
}

void PlaylistModel::find(const QString &path)
{
	//rowsAboutToBeInserted();
	beginResetModel();
	findRecursion(path, "*.*");
	endResetModel();
}

VideoFile PlaylistModel::at(const QModelIndex &idx) const
{
	return at(idx.row());
}

VideoFile PlaylistModel::at(int idx) const
{
	return m_vids.at(idx);
}

QJsonArray PlaylistModel::toJson() const
{
	QJsonArray arr;
	for (int i=0;i < m_vids.count();i++) {
		arr.append(m_vids.at(i).toJson());
	}
	return arr;
}

void PlaylistModel::fromJson(const QJsonArray &arr)
{
	beginResetModel();
	for (auto it=arr.constBegin(); it != arr.constEnd(); ++it) {
		m_vids << VideoFile::fromJson(it->toObject());
	}
	endResetModel();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	switch (role) {
	case Qt::EditRole:
		return at(index).fileName();
	case Qt::DisplayRole:
		return data(index, Qt::EditRole);
	}
	return QVariant();
}
