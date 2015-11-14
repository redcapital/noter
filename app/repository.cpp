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

		"CREATE VIRTUAL TABLE note_content USING fts5 (content, tokenize='unicode61', prefix='10,20');"

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
		// Cleanup stuff
		sqlite3_exec(this->database, "VACUUM", NULL, NULL, NULL);
		sqlite3_exec(this->database, "DELETE FROM tag WHERE id NOT IN (SELECT DISTINCT tag_id FROM tagging)", NULL, NULL, NULL);
	}
	this->loadTags();
	return true;
}

// Caller MUST free the returned pointer
ResultSet* Repository::search(const QString& query)
{
	assert(this->database);
	QString sql, ftsQuery;
	auto terms = this->parseQuery(query);
	bool isValidQuery = this->compileQuery(terms, sql, ftsQuery);
	if (!isValidQuery) {
		return new ResultSet(this);
	}
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
	if (!ftsQuery.isEmpty()) {
		code = sqlite3_bind_text(stmt, 1, ftsQuery.toUtf8().constData(), -1, SQLITE_TRANSIENT);
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

/**
 * Parse a search query. Note that we use a different, simpler syntax than FTS5
 *
 * Examples of queries:
 *
 * apple banana - containing "apple" and "banana"
 * ~apple ~banana ~lemon - containing either "apple", "banana" or "lemon"
 * "fresh apples" - containing the exact phrase "fresh apples"
 * -apple - not containing "apple"
 * #work - tagged with "work"
 * -#work - not tagged with "work"
 * app* - containing words that start with "app", e.g. "apples" or "application"
 * file ~word ~excel #work -#urgent - containing "file" and either "word" or "excel",
 *   and tagged with "work" but not "urgent"
 *
 *
 * Grammar in EBNF:
 *
 * query := { [ { whitespace } ] , term , [ { whitespace } ] } ;
 * term  := ['-' | '~'] , '"' , phrase , '"' ;
 * term  :=	['-' | '~'] , word ;
 * term  := ['-'] , '#' , word ;
 * phrase:= ? A sequence of any characters that is not a double quote ?;
 * word  := ? A sequence of any characters that is not a whitespace ? ;
 *
 */
list<Repository::QueryTerm> Repository::parseQuery(const QString& query) const
{
	list<Repository::QueryTerm> result;
	QRegExp termBoundary("\\s");
	int i = 0;
	while (i < query.length()) {
		if (query[i].isSpace()) {
			++i;
			continue;
		}
		Repository::QueryTerm term;
		if (query[i] == '-') {
			term.attributes |= Repository::QueryTerm::NEGATED;
			++i;
		} else if (query[i] == '~') {
			term.attributes |= Repository::QueryTerm::TILDE;
			++i;
		}

		if (i >= query.length()) {
			continue;
		}

		if (query[i] == '#' && (term.attributes & Repository::QueryTerm::TILDE) == 0) {
			term.attributes |= Repository::QueryTerm::TAG;
			++i;
		}

		if (i >= query.length()) {
			continue;
		}

		if (query[i] == '"' && (term.attributes & Repository::QueryTerm::TAG) == 0) {
			// Phrase term, consume everything till the closing double quote
			term.attributes |= Repository::QueryTerm::PHRASE;
			int pos = query.indexOf('"', i + 1);
			if (pos < 0) {
				term.body = query.mid(i + 1).trimmed();
				i = query.length();
			} else {
				term.body = query.mid(i + 1, pos - i - 1).trimmed();
				i = pos + 1;
			}
		} else {
			int pos = query.indexOf(termBoundary, i);
			if (pos < 0) {
				term.body = query.mid(i).trimmed();
				i = query.length();
			} else {
				term.body = query.mid(i, pos - i).trimmed();
				i = pos + 1;
			}
		}

		if (!term.body.isEmpty()) {
			result.push_back(term);
		}
	}
	return result;
}

/**
 * Take a list of parsed terms and compile them into a SQL query
 * @return whether the query might yield any result
 */
bool Repository::compileQuery(const std::list<QueryTerm>& terms, QString& sql, QString& ftsQuery) const
{
	QStringList normalTerms, tildeTerms, notTerms;
	QStringList includeTags, excludeTags;
	for (const auto &term : terms) {
		if (term.attributes & Repository::QueryTerm::TAG) {
			TagsByName::const_iterator it = tagsByName.find(Tag::normalizeName(term.body));
			if (term.attributes & Repository::QueryTerm::NEGATED) {
				if (it != tagsByName.constEnd()) {
					excludeTags.append(QString::number(it.value()->getId()));
				}
			} else {
				if (it == tagsByName.constEnd()) {
					// Tag doesn't exist, so search won't yield results
					return false;
				}
				includeTags.append(QString::number(it.value()->getId()));
			}
			continue;
		}

		QString ftsString;
		if (term.attributes & Repository::QueryTerm::PHRASE) {
			QStringList phraseWords;
			for (const auto &word : term.body.split(QRegExp("\\s"), QString::SkipEmptyParts)) {
				phraseWords.append(this->convertToFTSWord(word));
			}
			ftsString = QLatin1Char('(') + phraseWords.join(" + ") + QLatin1Char(')');
		} else {
			ftsString = this->convertToFTSWord(term.body);
		}

		if (term.attributes & Repository::QueryTerm::NEGATED) {
			notTerms.append(ftsString);
		} else if (term.attributes & Repository::QueryTerm::TILDE) {
			tildeTerms.append(ftsString);
		} else {
			normalTerms.append(ftsString);
		}
	}

	sql = "SELECT n.id, n.created_at AS createdAt, n.updated_at AS updatedAt, nc.content, GROUP_CONCAT(t.tag_id) AS tags "
		"FROM note n INNER JOIN note_content nc ON n.id = nc.rowid LEFT JOIN tagging t ON n.id = t.note_id ";
	ftsQuery.clear();

	QStringList where;
	bool matchApplied = false;
	if (!normalTerms.empty()) {
		ftsQuery.append(normalTerms.join(" AND "));
	}
	if (!tildeTerms.empty()) {
		if (!ftsQuery.isEmpty()) {
			ftsQuery.append(" AND ");
		}
		ftsQuery.append('(').append(tildeTerms.join(" OR ")).append(')');
	}
	if (!notTerms.isEmpty()) {
		if (ftsQuery.isEmpty()) {
			// Special case when only NOTs are present
			where.append("n.id NOT IN (SELECT rowid FROM note_content WHERE content MATCH :ftsQuery)");
			ftsQuery = notTerms.join(" OR ");
			matchApplied = true;
		} else {
			ftsQuery.append(" NOT ").append(notTerms.join(" NOT "));
		}
	}

	if (!excludeTags.isEmpty()) {
		where.append(
			QLatin1String("n.id NOT IN (SELECT DISTINCT note_id FROM tagging WHERE tag_id IN (") +
			excludeTags.join(',') +
			QLatin1String("))")
		);
	}
	if (!includeTags.isEmpty()) {
		QString condition(
			QLatin1String("n.id IN (SELECT note_id FROM tagging WHERE tag_id IN (") +
			includeTags.join(',') +
			QLatin1String(") GROUP BY note_id")
		);
		if (includeTags.size() > 1) {
			condition.append(" HAVING COUNT(*) = ").append(QString::number(includeTags.size()));
		}
		condition.append(')');
		where.append(condition);
	}

	if (!matchApplied && !ftsQuery.isEmpty()) {
		where.append("note_content MATCH :ftsQuery");
	}
	if (!where.isEmpty()) {
		sql.append(" WHERE ").append(where.join(" AND "));
	}
	sql.append(" GROUP BY n.id ORDER BY n.updated_at DESC");
	qDebug() << "SQL: " << sql;
	if (!ftsQuery.isEmpty()) {
		qDebug() << "FTS QUERY: " << ftsQuery;
	}
	return true;
}

/**
 * Converts a word from the parser into a FTS5 word
 *
 * @see http://www.sqlite.org/fts5.html#section_3
 */
QString Repository::convertToFTSWord(const QString& word) const
{
	bool needsEscaping = false;
	// Convert to lowercase, because some keywords like "AND" or "NEAR" have special meaning in FTS.
	// This won't affect search results in any way, since it's case insensitive
	QString result(word.toLower());
	for (auto i = 0; i < result.length(); i++) {
		ushort value = result[i].unicode();
		if (
			// Non-ASCII range characters
			value > 127 ||
			// Upper and lowercase ASCII characters
			(value > 64 && value < 91) || (value > 96 && value < 123) ||
			// Digits
			(value > 47 && value < 58) ||
			// Underscore and substitute characters
			(value == 96 || value == 26)
		) {
			// No need to escape these
			continue;
		}
		// Need to escape, with one exception: if '*' is used in a prefix
		// search, it doesn't need escaping, e.g. abc*
		needsEscaping = (i == 0 || i != (result.length() - 1) || value != 42);
		break;
	}
	if (!needsEscaping) {
		return result;
	}
	return QLatin1Char('"') + result.replace('"', "\"\"") + QLatin1Char('"');
}
