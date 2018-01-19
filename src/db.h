#ifndef DB_H
#define DB_H

#include <QtCore/qglobal.h>
#include <db_cxx.h>

class XHash;

class XDb
{
public:
	XDb();
	virtual ~XDb();
	void put(const QByteArray& key, const QByteArray& val);
	void setPath(const QString& path, const XHash& hash);
	QByteArray getPath(const QString& path);
	void close();
	void flush();

	static XDb* instance();
private:
	//DbEnv m_env;
	Db m_fs;
	Db m_paths;
	int m_numops = 0;
};

#endif // DB_H
