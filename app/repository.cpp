#include <cstdio>
#include <QUrl>
#include "repository.h"
#include <QDebug>

using namespace std;

QString Repository::getLastError() {
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

bool Repository::connect(QString filepath, bool isExisting) {
	this->lastError.clear();
	const char* localPath;
	if (filepath.startsWith("file:")) {
		localPath = QUrl(filepath).toLocalFile().toUtf8().constData();
	} else {
		localPath = filepath.toUtf8().constData();
	}
	if (!isExisting) {
		// Truncate file if creating
		FILE *handle = fopen(localPath, "w");
		if (handle == nullptr) {
			this->lastError = "Can't open the file";
			return false;
		}
		fclose(handle);
	}

	int error = sqlite3_open(localPath, &this->database);
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

vector<std::shared_ptr<Note>> Repository::findNotes(const QString& query)
{
	vector<std::shared_ptr<Note>> result;
	result.push_back(std::shared_ptr<Note>(new Note(1, 11, 22, "first note")));
	result.push_back(std::shared_ptr<Note>(new Note(2, 11, 22, "sec note")));
	result.push_back(std::shared_ptr<Note>(new Note(3, 11, 22, "third note")));
	//result.push_back(Note(2, 11, 22, "sec note"));
	//result.push_back(Note(3, 11, 22, "third note"));
	return result;
}

Repository::~Repository() {
	if (this->database != nullptr) {
		sqlite3_close(this->database);
	}
}
