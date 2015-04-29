#include <cstdio>
#include <cassert>
#include <ctime>
#include <QUrl>
#include "repository.h"
#include "resultset.h"

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
	const char statements[] =
		"CREATE TABLE config (key TEXT PRIMARY KEY, value TEXT);"

		"CREATE TABLE note (id INTEGER PRIMARY KEY, created_at INTEGER, updated_at INTEGER);"

		"CREATE VIRTUAL TABLE note_content USING fts4 (content, tokenize=unicode61, matchinfo=fts3, prefix=\"10,20\");"

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

		"CREATE INDEX idx_tag_id ON tagging(tag_id);"
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
	this->loadTags();
	return true;
}

// Caller MUST free the returned pointer
ResultSet* Repository::search(const QString& query)
{
	assert(this->database);
	QStringList normalTerms, tildeTerms, notTerms;
	QString includeTags, excludeTags;
	int includeTagsCount = 0;
	auto terms = parseQuery(query);
	for (const auto &term : terms) {
		if (term.type == Repository::QueryTerm::TERM_TAG) {
			TagsByName::const_iterator it = tagsByName.find(term.body);
			if (term.negated) {
				if (it != tagsByName.constEnd()) {
					if (!excludeTags.isEmpty()) {
						excludeTags.append(',');
					}
					excludeTags.append(QString::number(it.value()->getId()));
				}
			} else {
				if (it == tagsByName.constEnd()) {
					// Tag doesn't exist, return empty resultset and be done
					return new ResultSet(this);
				}
				if (!includeTags.isEmpty()) {
					includeTags.append(',');
				}
				includeTags.append(QString::number(it.value()->getId()));
				++includeTagsCount;
			}
			continue;
		}

		QString body = term.phrase ? ('"' + term.body + '"') : term.body;
		if (term.negated) {
			notTerms << body;
		} else if (term.type == Repository::QueryTerm::TERM_NORMAL) {
			normalTerms << body;
		} else {
			tildeTerms << body;
		}
	}
	QString sql(
		"SELECT n.id, n.created_at AS createdAt, n.updated_at AS updatedAt, nc.content, GROUP_CONCAT(t.tag_id) AS tags "
		"FROM note n INNER JOIN note_content nc ON n.id = nc.rowid LEFT JOIN tagging t ON n.id = t.note_id "
	);
	QString where, matchCondition;
	bool matchApplied = false;
	if (!normalTerms.empty()) {
		matchCondition.append(normalTerms.join(" AND "));
	}
	if (!tildeTerms.empty()) {
		if (!matchCondition.isEmpty()) {
			matchCondition.append(" AND ");
		}
		matchCondition.append('(').append(tildeTerms.join(" OR ")).append(')');
	}
	if (!notTerms.isEmpty()) {
		if (matchCondition.isEmpty()) {
			// Special case when only NOTs are present
			where.append("n.id NOT IN (SELECT rowid FROM note_content WHERE content MATCH :ftsQuery)");
			matchCondition = notTerms.join(" OR ");
			matchApplied = true;
		} else {
			matchCondition.append(" NOT ").append(notTerms.join(" NOT "));
		}
	}

	if (!excludeTags.isEmpty()) {
		if (!where.isEmpty()) {
			where.append(" AND ");
		}
		where.append("n.id NOT IN (SELECT DISTINCT note_id FROM tagging WHERE tag_id IN (").append(excludeTags).append("))");
	}
	if (!includeTags.isEmpty()) {
		if (!where.isEmpty()) {
			where.append(" AND ");
		}
		where.append("n.id IN (SELECT note_id FROM tagging WHERE tag_id IN (").append(includeTags).append(") GROUP BY note_id");
		if (includeTagsCount > 1) {
			where.append(" HAVING COUNT(*) = ").append(QString::number(includeTagsCount));
		}
		where.append(')');
	}

	if (!matchApplied && !matchCondition.isEmpty()) {
		if (!where.isEmpty()) {
			where.append(" AND ");
		}
		where.append("nc.content MATCH :ftsQuery");
	}
	if (!where.isEmpty()) {
		sql.append(" WHERE ").append(where);
	}
	sql.append(
		" GROUP BY n.id "
		" ORDER BY n.updated_at DESC"
	);
	qDebug() << "SQL: " << sql;

	QByteArray sqlBuffer = sql.toUtf8();
	sqlite3_stmt* stmt;
	int code = sqlite3_prepare_v2(
		this->database,
		sqlBuffer.constData(),
		-1,
		&stmt,
		NULL
	);
	if (code != SQLITE_OK) {
		qDebug() << "ERR PREPARE: " << sqlite3_errstr(code);
		// I don't expect this to happen, return empty for now
		return new ResultSet(this);
	}
	if (!matchCondition.isEmpty()) {
		matchCondition.insert(0, '\'');
		matchCondition.append('\'');
		qDebug() << "MATCH CONDITION: " << matchCondition;
		code = sqlite3_bind_text(stmt, 1, matchCondition.toUtf8().constData(), -1, SQLITE_TRANSIENT);
	}
	return new ResultSet(this, stmt);
}

void Repository::disconnect()
{
	sqlite3_close(this->database);
	this->database = nullptr;
	TagsById::const_iterator i = tagsById.constBegin();
	while (i != tagsById.constEnd()) {
		delete i.value();
		i++;
	}
	tagsById.clear();
	tagsByName.clear();
}

void Repository::loadTags()
{
	tagsById.clear();
	tagsByName.clear();
	assert(this->database);
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(
		this->database,
		"SELECT id, name FROM tag",
		-1,
		&stmt,
		NULL
	);
	int code;
	while ((code = sqlite3_step(stmt)) == SQLITE_ROW) {
		int id = sqlite3_column_int(stmt, 0);
		QString name((char*)sqlite3_column_text(stmt, 1));
		Tag *tag = new Tag(this, id, name);
		tagsById.insert(id, tag);
		tagsByName.insert(tag->getNormalizedName(), tag);
	}
	sqlite3_finalize(stmt);
}

Repository::~Repository()
{
	this->disconnect();
}

bool Repository::persistNote(Note *note)
{
	if (this->database == nullptr) {
		return false;
	}
	if (!note->isDirty()) {
		return true;
	}
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
	sqlite3_bind_int(stmt, 1, note->getUpdatedAt());
	sqlite3_bind_int(stmt, 2, note->getId());
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	sqlite3_exec(this->database, "COMMIT", NULL, NULL, NULL);
	note->resetDirty();
	return true;
}

// Caller MUST free the returned pointer
Note* Repository::createNote()
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
	return new Note(id, now, now, "");
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
	return true;
}

// The pointer is owned by the repository, caller must not free it
Tag* Repository::createTag(const QString& name)
{
	QString normalized(Tag::normalizeName(name));
	// Invalid name provided
	if (normalized.isEmpty()) {
		return nullptr;
	}
	TagsByName::const_iterator i = tagsByName.find(normalized);
	if (i != tagsByName.constEnd()) {
		return i.value();
	}
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(
		this->database,
		"INSERT INTO tag (name) VALUES (:name)",
		-1,
		&stmt,
		NULL
	);
	sqlite3_bind_text(stmt, 1, normalized.toUtf8().constData(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	int id = sqlite3_last_insert_rowid(this->database);
	Tag* created = new Tag(this, id, normalized);
	tagsById.insert(id, created);
	tagsByName.insert(created->getNormalizedName(), created);
	return created;
}

// Caller MUST free the returned pointer
TagList* Repository::autocompleteTag(const QString& name, const QList<int>& discardedIds)
{
	QString normalized(Tag::normalizeName(name));
	// Invalid name provided
	if (normalized.isEmpty()) {
		return nullptr;
	}
	std::vector<Tag*> list;
	int limit = 10;
	TagsById::const_iterator i = tagsById.constBegin();
	while (i != tagsById.constEnd()) {
		if (!discardedIds.contains(i.key()) && i.value()->nameStartsWith(normalized)) {
			list.push_back(i.value());
			if (--limit == 0) {
				break;
			}
		}
		i++;
	}
	return new TagList(list);
}

Tag* Repository::getTagById(int id)
{
	TagsById::const_iterator i = tagsById.constFind(id);
	if (i == tagsById.constEnd()) {
		return nullptr;
	}
	return i.value();
}

void Repository::addTag(Note* note, int tagId)
{
	Tag* tag = getTagById(tagId);
	if (tag) {
		note->addTag(tag);
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(
			this->database,
			"INSERT INTO tagging (note_id, tag_id) VALUES (:noteId, :tagId)",
			-1,
			&stmt,
			NULL
		);
		sqlite3_bind_int(stmt, 1, note->getId());
		sqlite3_bind_int(stmt, 2, tagId);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
}

void Repository::removeTag(Note* note, int tagId)
{
	Tag* tag = getTagById(tagId);
	if (tag) {
		note->removeTag(tag);
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(
			this->database,
			"DELETE FROM tagging WHERE note_id = :noteId AND tag_id = :tagId",
			-1,
			&stmt,
			NULL
		);
		sqlite3_bind_int(stmt, 1, note->getId());
		sqlite3_bind_int(stmt, 2, tagId);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
}

std::vector<Repository::QueryTerm> Repository::parseQuery(const QString& query) const
{
	std::vector<Repository::QueryTerm> result;
	int i = 0, pos;
	while (i < query.length()) {
		if (query[i].isSpace()) {
			++i;
			continue;
		}
		Repository::QueryTerm term;
		term.type = Repository::QueryTerm::TERM_NORMAL;
		if (query[i] == '-') {
			term.negated = true;
			++i;
		} else if (query[i] == '~') {
			term.type = Repository::QueryTerm::TERM_TILDE;
			++i;
		}
		if (query[i] == '#') {
			// See if tilde wasn't encountered
			if (term.type != Repository::QueryTerm::TERM_TILDE) {
				term.type = Repository::QueryTerm::TERM_TAG;
				++i;
			}
		}

		if (i >= query.length()) {
			continue;
		}

		if (query[i] == '"') {
			term.phrase = true;
			pos = query.indexOf('"', i + 1);
			if (pos < 0) {
				term.body = query.mid(i + 1);
				i = query.length();
			} else {
				term.body = query.mid(i + 1, pos - i - 1);
				i = pos + 1;
			}
		} else {
			pos = query.indexOf(QRegExp("\\s"), i);
			if (pos < 0) {
				term.body = query.mid(i);
				i = query.length();
			} else {
				term.body = query.mid(i, pos - i);
				i = pos + 1;
			}
		}

		if (term.type == Repository::QueryTerm::TERM_TAG) {
			term.body = Tag::normalizeName(term.body);
		} else {
			// Replace characters that may mess up FTS query syntax
			term.body.replace(QRegExp("[()\"':]"), " ");

			// Convert to lowercase, because some keywords like "AND" or "NEAR" have special meaning in FTS.
			// This won't affect search results in any way, since it's case insensitive
			term.body = term.body.trimmed().toLower();
		}

		if (!term.body.isEmpty()) {
			result.push_back(term);
		}
	}
	return result;
}
