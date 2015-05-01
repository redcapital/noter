#include <iostream>
#include <cstring>
#include "app.h"
#include "migrator.h"

int main(int argc, char *argv[])
{
	if (argc >= 2) {
		if (std::strncmp(argv[1], "migrate", 7) == 0) {
			if (argc < 4) {
				std::cerr << "Usage: migrate inputfile outputfile" << std::endl;
				return 1;
			}
			Migrator migrator;
			bool ok = migrator.migrate(argv[2], argv[3]);
			if (!ok) {
				std::cerr << migrator.getLastError().toLatin1().constData() << std::endl;
				return 1;
			}
			return 0;
		}
	}

	App app(argc, argv);
	return app.exec();
}
