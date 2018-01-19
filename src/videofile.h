#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include "fingerprint.h"
#include "xfile.h"

#include <QtAV/FrameReader.h>
#include <QtAV/Statistics.h>

class QJsonObject;


class VideoFile : public XFile
{
	Q_OBJECT

	typedef XFile Super;
public:
	enum MetaData {
		MD_Bitrate = 100,
	};

	VideoFile(const XHash& id, QObject *parent = nullptr);
	~VideoFile() Q_DECL_OVERRIDE;

	int stars() const {return m_stars;}
	void addStar() {m_stars++;}

	QJsonObject toJson() const Q_DECL_OVERRIDE;
	void loadJson(const QJsonObject& json) Q_DECL_OVERRIDE;

	void computeHash();

	Fingerprint fingerprint();
	QImage thumb();

	QVariant metaData(MetaData md);

	void setFromStats(const QtAV::Statistics& stats);
protected:
	QImage loadThumb(int pos);
	QtAV::FrameReader* reader();
private:
	int m_stars = 0;

	QByteArray m_hash;
	Fingerprint m_fing;
	QtAV::FrameReader* m_reader = nullptr;
	QImage m_thumb;
	QVariantMap m_meta;
};

#endif // VIDEOFILE_H
