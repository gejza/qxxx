#include "preferences.h"

Preferences::Preferences(QObject *parent) : QObject(parent)
{

}

Preferences* Preferences::instance()
{
	static Preferences pref;
	return &pref;
}

QString Preferences::dbPath() const
{
	return QStringLiteral("playlist.json");
}
