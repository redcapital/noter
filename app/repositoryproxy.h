#ifndef REPOSITORYPROXY_H
#define REPOSITORYPROXY_H

#include <QObject>
#include "repository.h"

class RepositoryProxy : public QObject {
	Q_OBJECT

private:
	Repository* repository;

public:
	RepositoryProxy(Repository* repository) : repository(repository) {}
	Q_INVOKABLE bool connectDatabase(const QString& filepath, bool isExisting);
	Q_INVOKABLE QString getLastError();
};

#endif // REPOSITORYPROXY_H
