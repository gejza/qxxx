#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QPointer>
#include "videofile.h"

class QJsonArray;
class VideoLibrary;

template<class T>
class SortedList : public QList<T>
{
public:
	typedef std::function<bool(const T&a, const T&b)> Sort;
	explicit SortedList() : fnc(0) {}
	SortedList(Sort f) : fnc(f) {}
	SortedList& operator = (const QList<T>& list) {
		QList<T>::operator= (list);
		if (fnc) {
			std::sort(QList<T>::begin(), QList<T>::end(), fnc);
		}
		return *this;
	}
	void setSort(Sort f) {
		fnc = f;
		std::sort(QList<T>::begin(), QList<T>::end(), fnc);
	}
private:
	Sort fnc;
};

template<typename T>
bool modelColumnCompareFunc(Qt::SortOrder order, const T&a, const T&b) {
	if (order == Qt::AscendingOrder)
		return a < b;
	else
		return a > b;
}

class PlaylistModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum { ColName, ColCreated, ColSize, ColFinger, ColThumb, ColCount, };

	explicit PlaylistModel(VideoLibrary* lib, QObject *parent = nullptr);

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	void sort(int column, Qt::SortOrder order) Q_DECL_OVERRIDE;
	bool canFetchMore(const QModelIndex &parent) const Q_DECL_OVERRIDE;
	void fetchMore(const QModelIndex &parent) Q_DECL_OVERRIDE;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	VideoFile* at(const QModelIndex& idx) const;
	VideoFile* at(int row) const;
	QModelIndex find(VideoFile* video) const;
private:
	QPointer<VideoLibrary> m_lib;
	SortedList<VideoFile*> m_items;
};

#endif // PLAYLISTMODEL_H
