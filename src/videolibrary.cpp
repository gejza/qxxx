#include "db.h"
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

VideoLibrary::VideoLibrary(QObject *parent)
	: Super(parent)
{

}

VideoLibrary *VideoLibrary::instance()
{
	static VideoLibrary lib;
	return &lib;
}

VideoFile *VideoLibrary::getVideo(const QString &path)
{
	XHash id = getHash(path);
	VideoFile* vf = m_vids.value(id, nullptr);
	if (vf) {
		vf->addPath(path);
		return vf;
	}
	vf = new VideoFile(id, this);
	vf->addPath(path);
	m_vids.insert(id, vf);
	return vf;
}

QList<VideoFile *> VideoLibrary::all() const
{
	return m_vids.values();
}

QJsonArray VideoLibrary::toJson() const
{
	QJsonArray arr;
	for (auto it=m_vids.begin(); it != m_vids.end(); ++it) {
		arr.append(it.value()->toJson());
	}
	return arr;
}

void VideoLibrary::fromJson(const QJsonArray &arr)
{
	for (auto it=arr.constBegin(); it != arr.constEnd(); ++it) {
		QJsonObject o = it->toObject();
		XHash h = XHash::fromByteArray(QByteArray::fromBase64(o.value("id").toString().toLatin1()));
		//QString p = o.value("path").toString();
		//XHash h = getHash(p);
		/*VideoFile* v = m_vids.value(h, nullptr);
		if (v) {
			qInfo() << "Add duplicate file" << v->filePath() << p;
			v->addPath(p);
			continue;
		}*/
		VideoFile* v = new VideoFile(h, this);
		v->loadJson(o);
		m_vids.insert(v->id(), v);
	}
	//if (bc != m_vids.count()) {
	//	emit filesAdded(bc-1, m_vids.count()-1);
	//}
}

void VideoLibrary::savePlaylist()
{
	saveData();
	QSaveFile f(Preferences::instance()->dbPath());
	QJsonDocument doc(toJson());
	if (f.open(QIODevice::WriteOnly)) {
		f.write(doc.toJson());
		f.commit();
	}
}

void VideoLibrary::saveData()
{
	for (auto it=m_vids.begin(); it != m_vids.end(); ++it) {
		it.value()->save();
	}
	XDb::instance()->flush();
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
		getVideo(prefix + match);
	}
	foreach (const QString &dir, currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
		findRecursion(prefix + dir, pattern);
}

void VideoLibrary::find(const QString &path)
{
	findRecursion(path, "*.*");
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

