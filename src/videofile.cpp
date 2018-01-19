#include "utils.h"
#include "videofile.h"

#include <QFileInfo>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>
#include <QThreadPool>
#define UNICODE
#include <MediaInfo/MediaInfo.h>


VideoFile::VideoFile(const XHash& id, QObject *parent)
	: Super(id, parent)
{

}

VideoFile::~VideoFile()
{

}

QJsonObject VideoFile::toJson() const
{
	QJsonObject ret = Super::toJson();
	if (stars()) {
		ret["stars"] = stars();
	}
	if (!m_meta.isEmpty()) {
		ret["meta"] = QJsonObject::fromVariantMap(m_meta);
	}
	return ret;
}

void VideoFile::loadJson(const QJsonObject &json)
{
	Super::loadJson(json);
	QJsonValue jsv = json.value("stars");
	if (!jsv.isUndefined()) {
		m_stars = jsv.toInt();
	}
}

void VideoFile::computeHash()
{
	if (m_hash.isEmpty()) {
		m_hash = Utils::genSha1(filePath());
	}
}

Fingerprint VideoFile::fingerprint()
{
	if (m_fing.isNull()) {
		m_fing = Fingerprint::fromFile(filePath());
	}
	return m_fing;
}

QImage VideoFile::thumb()
{
	if (m_thumb.isNull()) {
		//m_thumb = loadThumb(3000);
	}
	return m_thumb;
}

QVariant VideoFile::metaData(VideoFile::MetaData md)
{
	if (md == MD_Bitrate) {
		MediaInfoLib::MediaInfo mi;
		mi.Open(filePath().toStdWString());
		return QString::fromStdWString(mi.Get(MediaInfoLib::Stream_Video, 0,  L"BitRate"));
	}
	return QVariant();
}

void VideoFile::setFromStats(const QtAV::Statistics &stats)
{
	m_meta["duration"] = stats.duration;
	m_meta["format"] = stats.format;
	//for (auto it=stats.metadata.begin(); it != stats.metadata.end(); ++it) {
	//	m_meta[it.key()] = it.value();
	//}
}

QImage VideoFile::loadThumb(int pos)
{
	QtAV::FrameReader* r = reader();
	while (r->readMore()) {
		while (r->hasEnoughVideoFrames()) {
			const QtAV::VideoFrame f = r->getVideoFrame();
			qInfo() << "Found screen at" << f.timestamp();
			m_reader->deleteLater();
			m_reader = nullptr;
			return f.toImage();
		}
	}
	return QImage();
}

QtAV::FrameReader *VideoFile::reader()
{
	if (!m_reader) {
		m_reader = new QtAV::FrameReader();
		m_reader->setMedia(filePath());
	}
	return m_reader;
}
