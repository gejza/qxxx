#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>

typedef qint64 Pos;

struct Range {
	Pos min = 0;
	Pos max = -1;
	bool isValid() const {return min <= max;}
	Pos seek(qreal rpos) const;
	qreal ratio(Pos pos) const;
};

class TimelineNavigator;

class TimelineNavigator
{
public:
	virtual ~TimelineNavigator() {}
	virtual void paint(QPainter* painter) = 0;
	virtual Pos seekForward(const Range& total, Pos pos) = 0;
	virtual Pos seekBackward(const Range& total, Pos pos) = 0;
};

class TimeSeekNavigator : public TimelineNavigator
{
public:
	TimeSeekNavigator();
	~TimeSeekNavigator() Q_DECL_OVERRIDE;
	void paint(QPainter* painter) Q_DECL_OVERRIDE {}
	Pos seekForward(const Range& total, Pos pos) Q_DECL_OVERRIDE;
	Pos seekBackward(const Range& total, Pos pos) Q_DECL_OVERRIDE;

	Range finded;
};

class Timeline : public QWidget
{
	Q_OBJECT

public:
	enum SeekMode {
		MoveValue,
		FindFrame,
	};

	explicit Timeline(QWidget *parent = nullptr);

	void setBegin(Pos begin) { m_total.min = begin; updateGeometry(); }
	void setEnd(Pos end);
	void setPos(Pos pos) { m_pos = pos; update(); }

	QString toString() const;

	void markRangeBegin();
	void markRangeEnd();

	Pos seekForward();
	Pos seekBackward();
	void setSlowly();
	void setFaster();

	Q_SIGNAL void positionReqest(Pos pos);
protected:
	Pos posInTime(int pos) const;
	int posToWidgetPos(Pos pos) const;
	QRect centrerOf(Pos pos, int w) const;
	QRect centrerRange(Pos min, Pos max) const;
	//QRect makeRect(Pos from, Pos to) const;
	//void resizeEvent(QResizeEvent *resize_event) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent *paint_event) Q_DECL_OVERRIDE;
	//void wheelEvent(QWheelEvent *wheel_event) Q_DECL_OVERRIDE;
	//void mouseDoubleClickEvent(QMouseEvent *mouse_event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *mouse_event) Q_DECL_OVERRIDE;
	//void mouseMoveEvent(QMouseEvent *mouse_event) Q_DECL_OVERRIDE;
	//void mouseReleaseEvent(QMouseEvent *mouse_event) Q_DECL_OVERRIDE;
	//bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
private:
	TimelineNavigator* m_nav = nullptr;
	Pos m_pos = 0;
	Range m_total;
	Range m_find;
	SeekMode m_mode = MoveValue;
	Pos m_skipValue = 10000;

//	qint64 m_displayedRangeMin;
//	qint64 m_displayedRangeMax;
//	qint64 m_loadedRangeMin;
//	qint64 m_loadedRangeMax;
//	qint64 m_dataRangeMin;
//	qint64 m_dataRangeMax;
	QColor backgroundColor;
	QColor frameColor;
};

#endif // TIMELINE_H
