#include "resultset.h"

ResultSet::ResultSet(sqlite3_stmt* stmt_) : stmt(stmt_)
{
	doStep();
}

ResultSet::~ResultSet()
{
	sqlite3_finalize(stmt);
	if (lastResult != nullptr) {
		delete lastResult;
	}
}

void ResultSet::doStep()
{
	int code = sqlite3_step(stmt);
	if (code != SQLITE_ROW) {
		lastResult = nullptr;
		return;
	}
	int id = sqlite3_column_int(stmt, 0);
	int createdAt = sqlite3_column_int(stmt, 1);
	int updatedAt = sqlite3_column_int(stmt, 2);
	QString content((char*)sqlite3_column_text(stmt, 3));
	lastResult = new Note(id, createdAt, updatedAt, content);
}

bool ResultSet::hasMore()
{
	return lastResult != nullptr;
}

// Caller MUST free the returned pointer
Note* ResultSet::fetch()
{
	if (lastResult == nullptr) {
		return nullptr;
	}
	Note* result = lastResult;
	doStep();
	return result;
}
