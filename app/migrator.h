#ifndef MIGRATOR_H
#define MIGRATOR_H

#include <QString>

class Migrator
{
private:
	QString lastError;

public:
	bool migrate(const char* inputFile, const char* outputFile);
	QString getLastError() const { return lastError; }
};

#endif // MIGRATOR_H
