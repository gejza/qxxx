#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QObject>

class QSettings;

class Preferences : public QObject
{
	Q_OBJECT
public:
	explicit Preferences(QObject *parent = nullptr);

	static Preferences* instance();
	/**
	 * Provides access to the QSettings instance to allow storing/retrieving
	 * arbitrary values. The naming style for groups and keys is CamelCase.
	 */
	QSettings *settings() const { return m_settings; }
private:
	QSettings *m_settings = nullptr;
};

#endif // PREFERENCES_H
