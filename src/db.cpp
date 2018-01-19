#include "db.h"
#include "xhash.h"

#include <QDebug>

void
my_error_handler(const DbEnv *dbenv, const char *error_prefix,
	const char *msg)
{
  /*
   * Put your code to handle the error prefix and error
   * message here. Note that one or both of these parameters
   * may be NULL depending on how the error message is issued
   * and how the DB handle is configured.
   */
	qWarning() << error_prefix << msg;
}

XDb::XDb()
	: m_fs(NULL, 0), m_paths(nullptr, 0)
{
	m_fs.set_errcall(my_error_handler);
		m_fs.set_errpfx("my_example_program");
	u_int32_t oFlags = DB_CREATE;
	// Open the database
		m_fs.open(NULL,                // Transaction pointer
				"my_db.db",          // Database file name
				NULL,                // Optional logical database name
				DB_BTREE,            // Database access method
				oFlags,              // Open flags
				0);                  // File mode (using defaults)

		m_paths.open(NULL, "index.db", NULL, DB_BTREE, DB_CREATE, 0);
}

XDb::~XDb()
{
	m_fs.close(0);
	m_paths.close(0);
}

void XDb::put(const QByteArray &key, const QByteArray &val)
{
	Dbt k, d;
	k.set_data((void*)key.constData());
	k.set_size(key.size());
	//k.set_flags(DB_DBT_USERMEM);
	d.set_data((void*)val.constData());
	d.set_size(val.size());
	//d.set_flags(DB_DBT_USERMEM);
	m_fs.put(nullptr, &k, &d, 0);
}

void XDb::setPath(const QString &path, const XHash &hash)
{
	QByteArray k = path.toLatin1();
	QByteArray v = hash.toByteArray();
	Dbt kk((void*)k.constData(), k.size());
	Dbt vv((void*)v.constData(), v.size());
	m_paths.put(nullptr, &kk, &vv, 0);
	if (++m_numops > 10) {
		m_paths.sync(0);
		m_numops = 0;
	}
}

QByteArray XDb::getPath(const QString &path)
{
	QByteArray k = path.toLatin1();
	Dbt kk((void*)k.constData(), k.size());
	char buff[1024];
	Dbt dd;
	dd.set_data(buff);
	dd.set_ulen(sizeof(buff));
	dd.set_flags(DB_DBT_USERMEM);
	m_paths.get(NULL, &kk, &dd, 0);
	if (dd.get_size() > 0) {
		return QByteArray((const char*)dd.get_data(), dd.get_size());
	}
	return QByteArray();
}

void XDb::close()
{
	m_fs.close(0);
	m_paths.close(0);
}

void XDb::flush()
{
	m_fs.sync(0);
	m_paths.sync(0);
}

XDb *XDb::instance()
{
	static XDb _db;
	return &_db;
}
