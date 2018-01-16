#include "preferences.h"
#include "videolibrary.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSaveFile>
#include <QProgressDialog>
#include <QFutureWatcher>
#include <QThread>
#include <QtConcurrent>

VideoLibrary::VideoLibrary(QObject *parent) : QObject(parent)
{

}

VideoLibrary *VideoLibrary::instance()
{
	static VideoLibrary lib;
	return &lib;
}

void VideoLibrary::addFile(const QString &path)
{
	VideoFile* v = new VideoFile(m_vids.count()-1);
	v->setPath(path);
	m_vids.append(v);
	emit filesAdded(m_vids.count()-1, m_vids.count()-1);
}

QJsonArray VideoLibrary::toJson() const
{
	QJsonArray arr;
	for (int i=0;i < m_vids.count();i++) {
			arr.append(m_vids.at(i)->toJson());
	}
	return arr;
}

void VideoLibrary::fromJson(const QJsonArray &arr)
{
	int bc = m_vids.count();
	for (auto it=arr.constBegin(); it != arr.constEnd(); ++it) {
		m_vids.append(VideoFile::fromJson(it->toObject(), m_vids.count()));
	}
	//if (bc != m_vids.count()) {
	//	emit filesAdded(bc-1, m_vids.count()-1);
	//}
}

void VideoLibrary::savePlaylist()
{
	QSaveFile f(Preferences::instance()->dbPath());
	QJsonDocument doc(toJson());
	if (f.open(QIODevice::WriteOnly)) {
		f.write(doc.toJson());
		f.commit();
	}
}

void VideoLibrary::loadPlaylist()
{
	QFile f("playlist.json");
	if (f.open(QIODevice::ReadOnly)) {
		auto doc = QJsonDocument::fromJson(f.readAll());
		fromJson(doc.array());
	}
}

void VideoLibrary::findRecursion(const QString &path, const QString &pattern)
{
	QDir currentDir(path);
	const QString prefix = path + QLatin1Char('/');
	foreach (const QString &match, currentDir.entryList(QStringList(pattern), QDir::Files | QDir::NoSymLinks)) {
		addFile(prefix + match);
	}
	foreach (const QString &dir, currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
		findRecursion(prefix + dir, pattern);
}

void VideoLibrary::find(const QString &path)
{
	int bc = m_vids.count();
	findRecursion(path, "*.*");
	if (bc != m_vids.count()) {
		emit filesAdded(bc-1, m_vids.count()-1);
	}

}

int VideoLibrary::findPath(const QString &p)
{
	while (m_pathIndex.count() < m_vids.count()) {
		int rid = m_pathIndex.count();
		m_pathIndex.insert(getVideo(rid)->filePath(), rid);
	}
	return m_pathIndex.value(p, -1);
}

void VideoLibrary::computeHashes()
{
	// Create a progress dialog.
	QProgressDialog dialog;
	dialog.setLabelText(QString("Progressing using %1 thread(s)...").arg(QThread::idealThreadCount()));

	// Create a QFutureWatcher and connect signals and slots.
	QFutureWatcher<void> futureWatcher;
	QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
	QObject::connect(&dialog, &QProgressDialog::canceled, &futureWatcher, &QFutureWatcher<void>::cancel);
	QObject::connect(&futureWatcher,  &QFutureWatcher<void>::progressRangeChanged, &dialog, &QProgressDialog::setRange);
	QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged,  &dialog, &QProgressDialog::setValue);

	// Our function to compute
	std::function<void(VideoFile*)> spin = [](VideoFile* vf) {
		qInfo() << "iteration" << vf->fileName() << "in thread" << QThread::currentThreadId();
		vf->computeHash();
	};

	// Start the computation.
	futureWatcher.setFuture(QtConcurrent::map(m_vids, spin));

	// Display the dialog and start the event loop.
	dialog.exec();

	futureWatcher.waitForFinished();

	// Query the future to check if was canceled.
	qInfo() << "Canceled?" << futureWatcher.future().isCanceled();
}
