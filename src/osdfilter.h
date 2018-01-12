#ifndef OSDFILTER_H
#define OSDFILTER_H

#include <QtAV/Filter.h>
#include <QtAV/FilterContext.h>

namespace QtAV {
class Statistics;
}

class OSD
{
public:
	enum ShowType {
		ShowCurrentTime = 1,
		ShowCurrentAndTotalTime = 1<<1,
		ShowRemainTime = 1<<2,
		ShowPercent = 1<<3,
		ShowNone
	};

	OSD();
	virtual ~OSD();
	void setShowType(ShowType type);
	ShowType showType() const;
	void useNextShowType();
	bool hasShowType(ShowType t) const;
	QString text(QtAV::Statistics* statistics);
protected:
	ShowType mShowType;
	int mSecsTotal;
};

class OSDFilter : public QtAV::VideoFilter, public OSD
{
public:
	OSDFilter(QObject *parent = 0);
	bool isSupported(QtAV::VideoFilterContext::Type ct) const {
		return ct == QtAV::VideoFilterContext::QtPainter || ct == QtAV::VideoFilterContext::X11;
	}
protected:
	void process(QtAV::Statistics* statistics, QtAV::VideoFrame* frame);
};


#endif // OSDFILTER_H
