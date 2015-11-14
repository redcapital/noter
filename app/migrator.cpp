#include "sqlite3.h"
#include "migrator.h"
#include "repository.h"

bool Migrator::migrate(const char* inputFile, const char* outputFile)
{
	lastError.clear();
	Repository input, output;
	if (!input.connect(inputFile, true)) {
		lastError = input.getLastError();
		return false;
	}
	if (!output.connect(outputFile, false)) {
		lastError = output.getLastError();
		return false;
	}

	sqlite3* inputDb = input.getSqliteDatabase();
	sqlite3* outputDb = output.getSqliteDatabase();
	sqlite3_stmt *readStmt, *writeStmt;
	int code;

	// Migrate notes table
	sqlite3_prepare_v2(
		inputDb,
		"SELECT id, created_at, updated_at FROM note ORDER BY id",
		-1,
		&readStmt,
		NULL
	);
	sqlite3_prepare_v2(
		outputDb,
		"INSERT INTO note (id, created_at, updated_at) VALUES (:id, :c, :u)",
		-1,
		&writeStmt,
		NULL
	);
	while ((code = sqlite3_step(readStmt)) == SQLITE_ROW) {
		int id = sqlite3_column_int(readStmt, 0);
		int createdAt = sqlite3_column_int(readStmt, 1);
		int updatedAt = sqlite3_column_int(readStmt, 2);

		sqlite3_bind_int(writeStmt, 1, id);
		sqlite3_bind_int(writeStmt, 2, createdAt);
		sqlite3_bind_int(writeStmt, 3, updatedAt);
		sqlite3_step(writeStmt);
		sqlite3_reset(writeStmt);
	}

	sqlite3_finalize(readStmt);
	sqlite3_finalize(writeStmt);

	// Migrate tags
	sqlite3_prepare_v2(
		inputDb,
		"SELECT id, name FROM tag ORDER BY id",
		-1,
		&readStmt,
		NULL
	);
	sqlite3_prepare_v2(
		outputDb,
		"INSERT INTO tag (id, name) VALUES (:id, :name)",
		-1,
		&writeStmt,
		NULL
	);
	while ((code = sqlite3_step(readStmt)) == SQLITE_ROW) {
		int id = sqlite3_column_int(readStmt, 0);
		const char* name = (const char*)sqlite3_column_text(readStmt, 1);

		sqlite3_bind_int(writeStmt, 1, id);
		sqlite3_bind_text(writeStmt, 2, name, -1, NULL);
		sqlite3_step(writeStmt);
		sqlite3_reset(writeStmt);
	}

	sqlite3_finalize(readStmt);
	sqlite3_finalize(writeStmt);

	// Migrate taggings
	sqlite3_prepare_v2(
		inputDb,
		"SELECT note_id, tag_id FROM tagging",
		-1,
		&readStmt,
		NULL
	);
	sqlite3_prepare_v2(
		outputDb,
		"INSERT INTO tagging (note_id, tag_id) VALUES (:nid, :tid)",
		-1,
		&writeStmt,
		NULL
	);
	while ((code = sqlite3_step(readStmt)) == SQLITE_ROW) {
		int note_id = sqlite3_column_int(readStmt, 0);
		int tag_id = sqlite3_column_int(readStmt, 1);

		sqlite3_bind_int(writeStmt, 1, note_id);
		sqlite3_bind_int(writeStmt, 2, tag_id);
		sqlite3_step(writeStmt);
		sqlite3_reset(writeStmt);
	}

	sqlite3_finalize(readStmt);
	sqlite3_finalize(writeStmt);

	// Migrate content
	code = sqlite3_prepare_v2(
		inputDb,
		"SELECT rowid, content FROM note_content ORDER BY rowid",
		-1,
		&readStmt,
		NULL
	);
	if (code != SQLITE_OK) qDebug() << sqlite3_errstr(code);
	sqlite3_prepare_v2(
		outputDb,
		"UPDATE note_content SET content = :content WHERE rowid = :rowid",
		-1,
		&writeStmt,
		NULL
	);
	while ((code = sqlite3_step(readStmt)) == SQLITE_ROW) {
		int id = sqlite3_column_int(readStmt, 0);
		const char* content = (const char*)sqlite3_column_text(readStmt, 1);

		sqlite3_bind_text(writeStmt, 1, content, -1, NULL);
		sqlite3_bind_int(writeStmt, 2, id);
		sqlite3_step(writeStmt);
		sqlite3_reset(writeStmt);
	}

	sqlite3_finalize(readStmt);
	sqlite3_finalize(writeStmt);

	return true;
}
