#include "repositoryproxy.h"

bool RepositoryProxy::connectDatabase(const QString &filepath, bool isExisting) {
	return this->repository->connect(filepath, isExisting);
}

QString RepositoryProxy::getLastError() {
	return this->repository->getLastError();
}
