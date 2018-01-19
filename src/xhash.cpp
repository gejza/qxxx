#include "utils.h"
#include "xhash.h"
#include "xxhash.h"

#include <QElapsedTimer>
#include <QFile>
#include <QIODevice>
#include <QDebug>
#include <QFileInfo>
#include <QDataStream>

#define READ_SIZE 2*1024
#define SKIP_SIZE 2*1024*1024

namespace {
XXH64_hash_t calculateFromDev(QIODevice* dev)
{
	QElapsedTimer t;
	t.start();
	XXH64_state_t* state64 = XXH64_createState();
	XXH64_reset(state64, 0);
	char buff[READ_SIZE];
	while (!dev->atEnd()) {
		qint64 s = dev->read(buff, sizeof(buff));
		XXH64_update(state64, buff, s);
		break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
		dev->skip(SKIP_SIZE);
#else
		dev->seek(dev->pos()+SKIP_SIZE);
#endif
	}
	XXH64_hash_t ret = XXH64_digest(state64);
	XXH64_freeState(state64);
	qInfo() << "Compute " << dev->objectName() << ret << Utils::msToString(t.elapsed());
	return ret;
}
}

class XHashData : public QSharedData
{
public:
	quint8 ver = 0;
	XXH64_hash_t hash = 0;
};

XHash::XHash() : data(new XHashData)
{

}

XHash::XHash(const XHash &rhs) : data(rhs.data)
{

}

XHash &XHash::operator=(const XHash &rhs)
{
	if (this != &rhs)
		data.operator=(rhs.data);
	return *this;
}

XHash::~XHash()
{

}

QString XHash::toString() const
{
	if (data->hash == 0) {
		return QString("<none>");
	}
	return QString::number(data->hash);
}

QByteArray XHash::toByteArray() const
{
	QByteArray ret;
	QDataStream ds(&ret, QIODevice::WriteOnly);
	ds << data->ver;
	ds << data->hash;
	return ret;
}

XHash XHash::fromByteArray(const QByteArray &data)
{
	QDataStream ds(data);
	XHash ret;
	ds >> ret.data->ver;
	Q_ASSERT(ret.data->ver == 0x01);
	ds >> ret.data->hash;
	return ret;
}

bool XHash::operator ==(const XHash &o) const
{
	return data->hash == o.data->hash;
}

uint XHash::qHash(uint seed) const
{
	return ::qHash((quint64)data->hash, seed);
}

XHash XHash::fromFile(const QString &file_path)
{
	/*
	QFileInfo fi(file_path);
	XHash ret;
	ret.data->hash = fi.size();
	return ret;
	*/
	QFile f(file_path);
	if (f.open(QIODevice::ReadOnly)) {
		XHash ret;
		ret.data->ver = 0x01;
		ret.data->hash = calculateFromDev(&f);
		return ret;
	}
	return XHash();
}


