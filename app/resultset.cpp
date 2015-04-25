#include <vector>
#include <sstream>
#include "resultset.h"

ResultSet::ResultSet( Repository* _repository, sqlite3_stmt* _stmt) : repository(_repository), stmt(_stmt)
{
	if (_stmt) {
		doStep();
	}
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
	Note* note = new Note(id, createdAt, updatedAt, content);

	std::vector<Tag*> tags;
	const char* tagIdsString = (const char*)sqlite3_column_text(stmt, 4);
	if (tagIdsString) {
		int tagId;
		std::stringstream tagIds(tagIdsString);
		while (tagIds >> tagId) {
			if (tagIds.peek() == ',') {
				tagIds.ignore();
			}
			Tag* tag = repository->getTagById(tagId);
			if (!tag) {
				// Shouldn't happen
				qWarning() << "Tag isn't present in the repository " << tagId;
				continue;
			}
			tags.push_back(tag);
		}
	}
	note->setTags(new TagList(tags));
	lastResult = note;
}

bool ResultSet::hasMore()
{
	return stmt != nullptr && lastResult != nullptr;
}

// Caller MUST free the returned pointer
Note* ResultSet::fetch()
{
	if (stmt == nullptr || lastResult == nullptr) {
		return nullptr;
	}
	Note* result = lastResult;
	doStep();
	return result;
}
