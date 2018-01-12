#include "timeline.h"

#include <QPaintEvent>
#include <QPainter>
#include <QTime>

Timeline::Timeline(QWidget *parent) : QWidget(parent)
{
	backgroundColor = palette().base().color();
	frameColor = palette().mid().color();
	setMaximumHeight(30);
	setMinimumHeight(10);
	m_nav = new TimeSeekNavigator();
}

QString Timeline::toString() const
{
	return QString("%1:%2:%3").arg(QTime::fromMSecsSinceStartOfDay(m_find.min).toString()).arg(QTime::fromMSecsSinceStartOfDay(m_pos).toString()).arg(QTime::fromMSecsSinceStartOfDay(m_find.max).toString());
}

void Timeline::markRangeBegin()
{
	m_find.min = m_pos;
	updateGeometry();
}

void Timeline::markRangeEnd()
{
	m_find.max = m_pos;
	updateGeometry();
}

Pos Timeline::seekForward()
{
	if (m_nav) {
		return m_nav->seekForward(m_total, m_pos);
	}
	switch (m_mode) {
	case MoveValue:
		return m_pos + m_skipValue;
	case FindFrame:
		m_find.min = m_pos;
		return m_find.seek(0.3);
	};
	return m_pos + 1000;
}

Pos Timeline::seekBackward()
{
	if (m_nav) {
		return m_nav->seekBackward(m_total, m_pos);
	}
	switch (m_mode) {
	case MoveValue:
		return m_pos - m_skipValue;
	case FindFrame:
		m_find.max = m_pos;
		return m_find.seek(0.7);
	};
	return m_pos - 1000;
}

Pos Timeline::posInTime(int pos) const
{
	return m_total.seek(qreal(pos - rect().left()) / qreal(width() - rect().left()));
}

int Timeline::posToWidgetPos(Pos pos) const
{
	return rect().left() + (width() * m_total.ratio(pos));
}

QRect Timeline::centrerOf(Pos pos, int w) const
{
	QRect r = rect();
	r.setLeft(posToWidgetPos(pos)-w/2);
	r.setWidth(w);
	return r;
}

QRect Timeline::centrerRange(Pos min, Pos max) const
{
	QRect r = rect();
	r.setLeft(posToWidgetPos(min));
	r.setRight(posToWidgetPos(max));
	return r;
}

void Timeline::paintEvent(QPaintEvent *paint_event)
{
	QWidget::paintEvent(paint_event);

	QPainter painter(this);
	painter.fillRect(0, 0, width() - 1, height() - 1, backgroundColor); //??
	painter.save();
	painter.setPen(frameColor);
	painter.drawRect(0, 0, width() - 1, height() - 1);  //??
	painter.restore();

	QRect paint_rect = paint_event->rect();

	if (m_find.isValid()) {
		paint_rect.setLeft(posToWidgetPos(m_find.min));
		paint_rect.setRight(posToWidgetPos(m_find.max));
		painter.fillRect(paint_rect, Qt::green);
	}
	painter.save();
	painter.setBrush(Qt::red);
	//painter.drawEllipse(centrerOf(m_pos, 5));
	painter.drawEllipse(centrerRange(m_pos-m_skipValue, m_pos+m_skipValue));
	painter.restore();
}

void Timeline::mousePressEvent(QMouseEvent *mouse_event)
{
	QPoint pos = mouse_event->pos();
	if (mouse_event->buttons() & Qt::LeftButton) {
		emit positionReqest(posInTime(pos.x()));
	}

}

Pos Range::seek(qreal rpos) const
{
	return qBound(min, Pos((max - min) * rpos + min), max);

}

qreal Range::ratio(Pos pos) const
{
	return max > min ? qreal(pos - min) / qreal(max - min) : 0;
}

TimeSeekNavigator::TimeSeekNavigator()
{

}

TimeSeekNavigator::~TimeSeekNavigator()
{

}

Pos TimeSeekNavigator::seekForward(const Range &total, Pos pos)
{
	return pos + 15000;
}

Pos TimeSeekNavigator::seekBackward(const Range &total, Pos pos)
{
	return pos - 15000;
}
