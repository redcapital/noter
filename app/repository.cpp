#include <cstdio>
#include <cassert>
#include <ctime>
#include <QUrl>
#include "repository.h"
#include <QDebug>

using namespace std;

QString Repository::getLastError() const {
	return this->lastError;
}

bool Repository::checkSqliteError(int error) {
	if (error == SQLITE_OK) {
		return true;
	}
	this->lastError = sqlite3_errmsg(this->database);
	return false;
}

bool Repository::createSchema() {
	qDebug() << "Creating db schema";
	const char statements[] =
		"CREATE TABLE config (key TEXT PRIMARY KEY, value TEXT);"

		"CREATE TABLE note (id INTEGER PRIMARY KEY, created_at INTEGER, updated_at INTEGER);"

		"CREATE VIRTUAL TABLE note_content USING fts3 (content);"

		"CREATE TRIGGER note_delete AFTER DELETE ON note "
		"BEGIN "
		"DELETE FROM note_content WHERE rowid = OLD.id;"
		"END;"

		"CREATE TRIGGER note_insert AFTER INSERT ON note "
		"BEGIN "
		"INSERT INTO note_content (rowid, content) VALUES (NEW.id, '');"
		"END;"

		"CREATE TABLE tag (id INTEGER PRIMARY KEY, name TEXT UNIQUE);"

		"CREATE TABLE tagging ("
		"note_id INTEGER REFERENCES note(id) ON UPDATE CASCADE ON DELETE CASCADE, "
		"tag_id INTEGER REFERENCES tag(id) ON UPDATE CASCADE ON DELETE CASCADE, "
		"PRIMARY KEY (note_id, tag_id)"
		");"
	;
	return this->checkSqliteError(sqlite3_exec(this->database, statements, NULL, NULL, NULL));
}

void Repository::configureConnection() {
	const char statements[] =
		"PRAGMA foreign_keys = ON;"
		"PRAGMA synchronous = NORMAL;"
	;
	sqlite3_exec(this->database, statements, NULL, NULL, NULL);
}

bool Repository::validateSchema() {
	sqlite3_stmt* stmt;
	int status = sqlite3_prepare_v2(
		this->database,
		"SELECT 1 FROM sqlite_master WHERE type='table' AND name='note_content'",
		-1,
		&stmt,
		NULL
	);
	bool ok = this->checkSqliteError(status);
	if (ok) {
		switch (sqlite3_step(stmt)) {
		case SQLITE_DONE:
			// No rows returned
			this->lastError = "Not a Noter database";
			ok = false;
			break;
		case SQLITE_ROW:
			// The table is there, therefore database seems good
			break;
		default:
			ok = this->checkSqliteError(status);
		}
	}
	sqlite3_finalize(stmt);
	return ok;
}

bool Repository::connect(QString filepath, bool isExisting)
{
	this->disconnect();
	this->lastError.clear();
	QByteArray localPath;
	if (filepath.startsWith("file:")) {
		localPath = QUrl(filepath).toLocalFile().toUtf8();
	} else {
		localPath = filepath.toUtf8();
	}
	if (!isExisting) {
		// Truncate file if creating
		FILE *handle = fopen(localPath.constData(), "w");
		if (handle == nullptr) {
			this->lastError = "Can't open the file";
			return false;
		}
		fclose(handle);
	}

	int error = sqlite3_open_v2(localPath.constData(), &this->database, SQLITE_OPEN_READWRITE, NULL);
	if (!this->checkSqliteError(error)) {
		return false;
	}

	this->configureConnection();
	if (!isExisting) {
		if (!this->createSchema()) {
			return false;
		}
	} else {
		if (!this->validateSchema()) {
			return false;
		}
	}
	return true;
}

Repository::ResultSetPtr Repository::findNotes(const QString& query)
{
	assert(this->database);
	ResultSetPtr result(new ResultSet);
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(
		this->database,
		"SELECT n.id, n.created_at AS createdAt, n.updated_at AS updatedAt, nc.content, GROUP_CONCAT(t.tag_id) AS tags "
		"FROM note n INNER JOIN note_content nc ON n.id = nc.rowid LEFT OUTER JOIN tagging t ON n.id = t.note_id "
		"WHERE nc.content LIKE :content GROUP BY n.id "
		"ORDER BY n.updated_at DESC",
		-1,
		&stmt,
		NULL
	);
	QByteArray buffer = query.toUtf8();
	buffer.insert(0, '%');
	buffer.append('%');
	sqlite3_bind_text(stmt, 1, buffer.constData(), -1, NULL);

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		int id = sqlite3_column_int(stmt, 0);
		int createdAt = sqlite3_column_int(stmt, 1);
		int updatedAt = sqlite3_column_int(stmt, 2);
		QString content((char*)sqlite3_column_text(stmt, 3));
		result->push_back(make_shared<Note>(id, createdAt, updatedAt, content));
	}
	sqlite3_finalize(stmt);
	return result;
}

void Repository::disconnect()
{
	sqlite3_close(this->database);
	this->database = nullptr;
}

Repository::~Repository()
{
	this->disconnect();
}

bool Repository::updateNote(Note *note)
{
	if (this->database == nullptr) {
		return false;
	}
	if (!note->isDirty()) {
		return true;
	}
	time_t now = time(nullptr);
	sqlite3_exec(this->database, "BEGIN", NULL, NULL, NULL);
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(
		this->database,
		"UPDATE note_content SET content = :content WHERE rowid = :id",
		-1,
		&stmt,
		NULL
	);
	sqlite3_bind_text(stmt, 1, note->getContent().toUtf8().constData(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 2, note->getId());
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	sqlite3_prepare_v2(
		this->database,
		"UPDATE note SET updated_at = :updatedAt WHERE id = :id",
		-1,
		&stmt,
		NULL
	);
	sqlite3_bind_int(stmt, 1, now);
	sqlite3_bind_int(stmt, 2, note->getId());
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	sqlite3_exec(this->database, "COMMIT", NULL, NULL, NULL);
	note->setUpdatedAt(now);
	note->resetDirty();
	emit noteUpdated(note);
	return true;
}

bool Repository::createNote()
{
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(
		this->database,
		"INSERT INTO note (created_at, updated_at) VALUES (:createdAt, :updatedAt)",
		-1,
		&stmt,
		NULL
	);
	time_t now = time(nullptr);
	sqlite3_bind_int(stmt, 1, now);
	sqlite3_bind_int(stmt, 2, now);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	int id = sqlite3_last_insert_rowid(this->database);
	auto note = make_shared<Note>(id, now, now, "");
	emit noteCreated(note);
	return true;
}

bool Repository::deleteNote(Note *note)
{
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(
		this->database,
		"DELETE FROM note WHERE id = :id",
		-1,
		&stmt,
		NULL
	);
	sqlite3_bind_int(stmt, 1, note->getId());
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	emit noteDeleted(note);
	return true;
}
